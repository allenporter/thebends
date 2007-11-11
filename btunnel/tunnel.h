// tunnel.h
// Autor: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_TUNNEL_H__
#define __BTUNNEL_TUNNEL_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

namespace yhttpserver {
  class Select;
}

namespace btunnel {

class Packer;

class Tunnel {
 public:
  virtual ~Tunnel() { }

  virtual void Start() = 0;
  virtual void Stop() = 0;

 protected:
  // Cannot be instantiated directly
  Tunnel() { }
};

Tunnel* NewTcpTunnel(yhttpserver::Select* select,
                     struct in_addr remote_addr,
                     uint16_t remote_port,
                     uint16_t local_port);

}  // namespace btunnel

#endif  // __BTUNNEL_TUNNEL_H__
