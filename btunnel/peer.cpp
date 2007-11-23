// peer.cpp
// Author: Allen Porter <allen@thebends.org>

#include <err.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sysexits.h>
#include <unistd.h>
#include <ynet/select.h>
#include <map>

#include "peer.h"
#include "buffer.h"
#include "peer_message.h"

using namespace std;

#define min(a, b) (((a) < (b)) ? (a) : (b))

namespace btunnel {

static const int kBufferSize = 512 * 1024;  // 512k

//
// Default peer implementation
//

bool Peer::Register(int sock, const RegisterRequest* request) {
  errx(1, "Register not implemented");
  return false;
}

bool Peer::Unregister(int sock, const UnregisterRequest* request) {
  errx(1, "Unregister not implemented");
  return false;
}

bool Peer::Forward(int sock, const ForwardRequest* request) {
  errx(1, "Forward not implemented");
  return false;
}

class PeerImpl : public Peer {
 public:
  virtual bool Register(int client_sock, const RegisterRequest* request) {
    // register the service for clients
    return false;
  }

  virtual bool Unregister(int client_sock, const UnregisterRequest* request) {
    // remove the service
    return false;
  }

  virtual bool Forward(int client_sock, const ForwardRequest* request) {
    // lookup service; maybe create a new connection
    return false;
  }
};

// TODO: Server peer vs client peer
Peer* NewPeer(const vector<btunnel::Service*>& services) {
  return new PeerImpl();
}


class ClientPeer : public Peer {
 public:
  ClientPeer(Buffer* buffer) : buffer_(buffer) { }

  virtual ~ClientPeer() { }

  virtual bool Register(int client_sock, const RegisterRequest* request) {
    WriteRegister(buffer_, *request);
    return true;
  }

  virtual bool Unregister(int client_sock, const UnregisterRequest* request) {
    WriteUnregister(buffer_, *request);
    return true;
  }

  virtual bool Forward(int client_sock, const ForwardRequest* request) {
    WriteForward(buffer_, *request);
    return true;
  }

  Buffer* buffer_;
};

Peer* NewClientPeer(Buffer* buffer) {
  return new ClientPeer(buffer);
}

}  // namespace btunnel
