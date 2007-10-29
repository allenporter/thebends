// server.c
// Author: Allen Porter <allen@thebends.org>
#include <assert.h>
#include <arpa/inet.h>
#include <err.h>
#include <limits.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>

static int udp_listen(int port) {
  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    err(1, "socket");
  }
  // TODO: Reuse port?
  struct sockaddr_in name;
  socklen_t namelen = sizeof(struct sockaddr_in);
  bzero(&name, namelen);
  name.sin_addr.s_addr = htonl(INADDR_ANY);
  name.sin_port = htons(port);
  if (bind(sock, (const struct sockaddr*)&name, namelen) != 0) {
    err(1, "bind");
  } 
  return sock;
}

static void log_packet(const struct sockaddr_in* name,
                       int received, size_t bytes) {
  printf("%ld: %c %s:%d %ld\n", time(NULL), (received ? '>' : '<'),
         inet_ntoa(name->sin_addr), name->sin_port, bytes);
  fflush(stdout);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    errx(EX_USAGE, "Usage: %s <port> <interval>", argv[0]);
  }
  uint16_t port = strtol(argv[1], (char**)NULL, 10);
  struct timeval timeout;
  memset(&timeout, 0, sizeof(struct timeval));
  timeout.tv_sec = strtol(argv[2], (char**)NULL, 10);

  int s = udp_listen(port);
  struct sockaddr_in addr;
  socklen_t addr_len = sizeof(struct sockaddr_in);
  bzero(&addr, addr_len);
  ssize_t nread = 0;
  fd_set readfds;
  char buf[BUFSIZ];
  size_t buf_len = BUFSIZ;
  unsigned int count = 0;
  while (1) {
    // select() can either return because the timeout passed, or a packet is
    // ready to be received (and we have a new destination)
    FD_ZERO(&readfds);
    FD_SET(s, &readfds);
    if (select(s + 1, &readfds, NULL, NULL, &timeout) == -1) {
      err(1, "select");
    }
    if (FD_ISSET(s, &readfds)) {
      // Incoming packet was received, and the source is now the new destination
      // for outgoing echos.
      if ((nread = recvfrom(s, buf, buf_len - 1, MSG_WAITALL,
                            (struct sockaddr*)&addr, &addr_len)) == -1) {
        err(1, "recvfrom");
      }
      assert(nread > 0);
      assert(addr_len == sizeof(struct sockaddr_in));
      log_packet(&addr, 1, nread);
      buf[nread] = '\0';
      count = 0;
    } 
    if (nread > 0) {  // Ever received a packet?
      char out[BUFSIZ];
      snprintf(out, BUFSIZ, "%ld: %d %s", time(NULL), count++, buf);
      size_t out_len = strlen(out);
      ssize_t nsent = 0;
      if ((nsent = sendto(s, out, out_len, 0, (const struct sockaddr*)&addr,
                          addr_len)) == -1) {
        err(1, "sendto");
      }
      assert(nsent == out_len);
      log_packet(&addr, 0, nsent);
    }
  }
  // not reached
  return 0;
}
