// tunnel.h
// Autor: Allen Porter <allen@thebends.org>
//
// Tunnel forwards incoming connections on a local port to a remote host/port.

#ifndef __YNET_TUNNEL_H__
#define __YNET_TUNNEL_H__

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

namespace ynet {

class Select;

class Tunnel {
 public:
  virtual ~Tunnel() { }

  virtual void Start() = 0;
  virtual void Stop() = 0;

 protected:
  // Cannot be instantiated directly
  Tunnel() { }
};

Tunnel* NewTcpTunnel(Select* select,
                     struct in_addr remote_addr,
                     uint16_t remote_port,
                     uint16_t local_port);

}  // namespace ynet

#endif  // __YNET_TUNNEL_H__
