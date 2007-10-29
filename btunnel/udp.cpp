#include "udp.h"

#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

namespace btunnel {

class UdpSocket : public Socket {
 public:
  UdpSocket(uint16_t port) : port_(port) {
    Setup();
  }

  virtual ~UdpSocket() {
    close(sock_);
  }

 private:
  void Setup() {
    struct sockaddr_in sin;
    unsigned int opt = 1;
    if ((sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      err(EX_OSERR, "socket (tunnel)");
    }
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, &opt,
                   sizeof(opt)) == -1 ) {
      err(EX_OSERR, "setsockopt:SO_REUSEPORT (tunnel)");
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

  uint16_t port_;

};

Socket* NewUdpSocket(uint16_t port) {
  return new UdpSocket(port);
}

}  // namespace btunnel
