// client.c
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

static void log_packet(const struct sockaddr_in* name,
                       int received, size_t bytes) {
  printf("%ld: %c %s:%d %ld\n", time(NULL), (received ? '>' : '<'),
         inet_ntoa(name->sin_addr), htons(name->sin_port), bytes);
  fflush(stdout);
}

static void usage(char* argv[]) {
  errx(EX_USAGE, "Usage: %s <ip> <port>", argv[0]);
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    usage(argv);
  }
  const char* ip = argv[1];
  uint16_t port = strtol(argv[2], (char**)NULL, 10);

  struct sockaddr_in name;
  socklen_t namelen = sizeof(struct sockaddr_in);
  bzero(&name, namelen);
  if (inet_aton(ip, &name.sin_addr) != 1) {
    usage(argv);
  } 
  name.sin_family = AF_INET;
  name.sin_port = htons(port);

  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    err(1, "socket");
  }

  char buf[BUFSIZ];
  strncpy(buf, "some payload", BUFSIZ);
  ssize_t buf_len = strlen(buf);

  ssize_t nsent;
  if ((nsent = sendto(sock, buf, buf_len, 0,
                      (const struct sockaddr*)&name, namelen)) == -1) {
    err(1, "sendto");
  }
  log_packet(&name, 0, nsent);

  while (1) {
    ssize_t nread;
    if ((nread = recvfrom(sock, buf, BUFSIZ - 1, MSG_WAITALL,
                          (struct sockaddr*)&name, &namelen)) == -1) {
      err(1, "recvfrom");
    }
    assert(nread > 0);
    assert(namelen = sizeof(struct sockaddr_in));
    log_packet(&name, 1, nread);
  }
  return 0;
}
