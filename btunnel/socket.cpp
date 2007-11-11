#include "socket.h"

#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

namespace btunnel {

Socket::Socket() { }

Socket::~Socket() { }

int Socket::sock() {
  return sock_;
}

SocketBase::SocketBase(uint16_t port) : port_(port) {
  Setup();
}

SocketBase::~SocketBase() {
  close(sock_);
}

void SocketBase::Setup() {
  struct sockaddr_in sin;
  unsigned int opt = 1;
  if ((sock_ = create_socket()) == -1) {
    err(EX_OSERR, "socket");
  }
  if (setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, &opt,
                 sizeof(opt)) == -1 ) {
    err(EX_OSERR, "setsockopt:SO_REUSEPORT");
  }

  bzero(&sin, sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = htonl(INADDR_ANY);
  sin.sin_port = htons(port_);

  if (bind(sock_, (struct sockaddr *)&sin,
           sizeof(struct sockaddr_in)) == -1 ) {
    err(EX_OSERR, "bind");
  }
}

}  // namespace btunnel
