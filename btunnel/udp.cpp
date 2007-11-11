// udp.cpp
// Author: Allen Porter <allen@thebends.org>

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

class UdpSocket : public SocketBase {
 public:
  UdpSocket(uint16_t port) : SocketBase(port) { }
  virtual ~UdpSocket() { }

 protected:
  int create_socket() {
    return socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  }
};

Socket* NewUdpSocket(uint16_t port) {
  return new UdpSocket(port);
}

}  // namespace btunnel
