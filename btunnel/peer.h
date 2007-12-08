// peer.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __BTUNNEL_PEER_H__
#define __BTUNNEL_PEER_H__

#include <vector>
#include <map>
#include "peer_message.h"

namespace ynet { class WriteBuffer; }

namespace btunnel {

class Service;

class Peer {
 public:
  virtual ~Peer() { }

  virtual bool Register(int sock, const RegisterRequest* request);

  virtual bool Unregister(int sock, const UnregisterRequest* request);

  virtual bool Forward(int sock, const ForwardRequest* request);

 protected:
  // Cannot be instantiated directly
  Peer() { }
};

Peer* NewPeerWriter(ynet::WriteBuffer* buffer);

}  // namespace btunnel

#endif  // __BTUNNEL_PEER_H__
