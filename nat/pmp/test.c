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

#define UPMP_PORT 5351

struct discover_response {
  uint8_t vers;
  uint8_t op;
  uint16_t result;
  uint32_t epoch;
  struct in_addr ip;
};

static void usage(char* argv[]) {
  errx(EX_USAGE, "Usage: %s <ip>", argv[0]);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    usage(argv);
  }
  const char* ip = argv[1];

  struct sockaddr_in name;
  socklen_t namelen = sizeof(struct sockaddr_in);
  bzero(&name, namelen);
  if (inet_aton(ip, &name.sin_addr) != 1) {
    usage(argv);
  } 
  name.sin_family = AF_INET;
  name.sin_port = htons(UPMP_PORT);

  int sock;
  if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
    err(1, "socket");
  }

  ssize_t buf_len = 2;
  char buf[BUFSIZ];
  bzero(buf, BUFSIZ);

  ssize_t nsent;
  if ((nsent = sendto(sock, buf, buf_len, 0,
                      (const struct sockaddr*)&name, namelen)) == -1) {
    err(1, "sendto");
  }

  struct discover_response resp;
  ssize_t nread;
  if ((nread = recvfrom(sock, &resp, sizeof(struct discover_response),
                        0, (struct sockaddr*)&name, &namelen)) == -1) {
    err(1, "recvfrom");
  }
  assert(nread > 0);
  assert(namelen = sizeof(struct sockaddr_in));

  printf("vers=%d\n", resp.vers);
  printf("op=%d\n", resp.op);
  printf("result=%d\n", resp.result);
  printf("uptime=%u\n", ntohl(resp.epoch));
  printf("ip=%s\n", inet_ntoa(resp.ip));
  return 0;
}
