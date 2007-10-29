// tunnel.h
// Autor: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_TUNNEL_H__
#define __BTUNNEL_TUNNEL_H__

#include <sys/types.h>
#include "listener.h"

namespace btunnel {

class Packer;

class Tunnel {
 public:
  virtual ~Tunnel() { }

  virtual void Write(const unsigned char* data,
                     size_t length) = 0;

 protected:
  // Cannot be instantiated directly
  Tunnel() { }
};

Tunnel* NewUdpTunnel(struct in_addr remote_addr,
                     uint16_t remote_port,
                     uint16_t local_port,
                     Packer* packer,
                     ReceivedCallback* callback);

}  // namespace btunnel

#endif  // __BTUNNEL_TUNNEL_H__
