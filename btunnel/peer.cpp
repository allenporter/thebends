// peer.cpp
// Author: Allen Porter <allen@thebends.org>

#include <err.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sysexits.h>
#include <unistd.h>
#include <ynet/select.h>
#include <ynet/buffer.h>
#include <map>

#include "peer.h"
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

class PeerMessageWriter : public Peer {
 public:
  PeerMessageWriter(ynet::WriteBuffer* buffer) : buffer_(buffer) { }

  virtual ~PeerMessageWriter() { }

  virtual bool Register(int client_sock, const RegisterRequest* request) {
    if (WriteRegister(buffer_, *request) < 0) {
      return false;
    }
    return true;
  }

  virtual bool Unregister(int client_sock, const UnregisterRequest* request) {
    if (WriteUnregister(buffer_, *request) < 0) {
      return false;
    }
    return true;
  }

  virtual bool Forward(int client_sock, const ForwardRequest* request) {
    if (WriteForward(buffer_, *request) < 0) {
      return false;
    }
    return true;
  }

  ynet::WriteBuffer* buffer_;
};

Peer* NewPeerWriter(ynet::WriteBuffer* buffer) {
  return new PeerMessageWriter(buffer);
}

}  // namespace btunnel
