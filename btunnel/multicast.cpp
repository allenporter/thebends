// multicast.cpp
// Auhor: Allen Porter <allen@thebends.org>
#include "multicast.h"

#include <assert.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sysexits.h>
#include <strings.h>
#include <unistd.h>
#include <ythread/mutex.h>

namespace btunnel {

class MulticastSocket : public Socket {
 public:
  MulticastSocket(uint32_t multicast_addr, uint16_t port)
    : multicast_addr_(multicast_addr),
      port_(port) {
    Setup();
  }

  virtual ~MulticastSocket() {
    Teardown();
  }

 private:
  void Setup() {
    struct sockaddr_in sin;
    struct ip_mreq mcast_cmd;
    unsigned int opt = 1;
    unsigned char ttl = 1;

    if ((sock_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
      err(EX_OSERR, "socket (mcast)");
    }
    if (setsockopt(sock_, SOL_SOCKET, SO_REUSEPORT, &opt,
          sizeof(opt)) == -1) {
      err(EX_OSERR, "setsockopt:SO_REUSEPORT (mcast)");
    }
    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_LOOP, &opt,
          sizeof(opt)) < 0) {
      err(EX_OSERR, "setsockopt:IP_MULTICAST_LOOP (mcast)");
    }
    if (setsockopt(sock_, IPPROTO_IP, IP_MULTICAST_TTL, (char *)&ttl,
          sizeof(ttl)) < 0) {
      err(EX_OSERR, "setsockopt:IP_MULTICAST_TTL (mcast)");
    }

    // multicast group
    struct sockaddr_in sin_mcast;
    bzero(&sin_mcast, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port_);

    mcast_cmd.imr_multiaddr.s_addr = htonl(multicast_addr_);
    mcast_cmd.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock_, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mcast_cmd,
          sizeof(mcast_cmd)) < 0 ) {
      err(EX_OSERR, "setsockopt:IP_ADD_MEMBERSHIP (mcast)");
    }
    if (bind(sock_, (struct sockaddr *)&sin,
          sizeof(struct sockaddr_in)) == -1 ) {
      err(EX_OSERR, "bind");
    }
  }

  void Teardown() {
    struct ip_mreq mcast_cmd;
    mcast_cmd.imr_multiaddr.s_addr = htonl(multicast_addr_);
    mcast_cmd.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sock_, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mcast_cmd,
          sizeof(mcast_cmd)) < 0) {
      err(EX_OSERR, "setsockopt:IP_DROP_MEMBERSHIP");
    }
    close(sock_);
  }

  uint32_t multicast_addr_;
  uint16_t port_; 
};

Socket* NewMulticastSocket(uint32_t multicast_addr, uint16_t port) {
  return new MulticastSocket(multicast_addr, port);
}


}  // namespace btunnel
