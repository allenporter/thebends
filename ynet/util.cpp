// util.cpp
// Author: Allen Porter <allen@thebends.org>

#include "util.h"
#include <assert.h>
#include <arpa/inet.h>
#include <err.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <strings.h>

namespace ynet {

struct sockaddr_in SockAddr(const char* ip, int16_t port) {
  struct sockaddr_in addr;
  bzero(&addr, sizeof(struct sockaddr_in));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_aton(ip, &addr.sin_addr);
  return addr;
}

void SetNonBlocking(int sock) {
  if ((fcntl(sock, F_SETFL, O_NONBLOCK)) == -1) {
    err(1, "fcntl");
  }
}

}  // namespace ynet

