// public-ip.c
// Author: Allen Porter <allen@thebends.org>
//
// Simple tool that makes a NAT PMP request to your gateway, and returns the
// public IP of the router.
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
#include "route.h"

#define UPMP_PORT 5351

struct discover_response {
  uint8_t vers;
  uint8_t op;
  uint16_t result;
  uint32_t epoch;
  struct in_addr ip;
};

int main(int argc, char* argv[]) {
  struct sockaddr_in name;
  socklen_t namelen = sizeof(struct sockaddr_in);
  bzero(&name, namelen);
  if (!pmp::GetDefaultGateway(&name.sin_addr)) {
    errx(1, "GetDefaultRoute failed");
  }
  name.sin_family = AF_INET;
  name.sin_port = htons(UPMP_PORT);

  printf("Sending nat-pmp request to Gateway: %s\n", inet_ntoa(name.sin_addr));

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

  // TODO: Retry requests and do non-blocking recv
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
