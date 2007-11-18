// peer.cpp
// Author: Allen Porter <allen@thebends.org>

#include "peer.h"
#include <err.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sysexits.h>
#include <unistd.h>
#include <yhttp/select.h>
#include <map>
#include "buffer.h"
#include "peer_message.h"

#define min(a, b) (((a) < (b)) ? (a) : (b))

namespace btunnel {

static const int kBufferSize = 512 * 1024;  // 512k

PeerConnection::PeerConnection(yhttpserver::Select* select)
    : select_(select) {
}

PeerConnection::~PeerConnection() { }

void PeerConnection::AddPeer(int sock) {
  yhttpserver::Select::AcceptCallback* cb =
    ythread::NewCallback(this, &PeerConnection::Read);
  select_->AddFd(sock, cb);
  peers_[sock] = new Buffer(kBufferSize);
}

void PeerConnection::RemovePeer(int sock) {
  Close(sock);
}

Buffer* PeerConnection::GetBuffer(int sock) {
  PeerMap::iterator iter = peers_.find(sock);
  assert(iter != peers_.end());   
  return iter->second;
}

void PeerConnection::Read(int sock) {
  // Throw the pending data from the client into the buffer
  Buffer* buffer = GetBuffer(sock);
  char buf[BUFSIZ];
  ssize_t nread = read(sock, buf, BUFSIZ);
  if (nread == -1) {
    err(EX_OSERR, "read() (%d)", sock);
  } else if (nread == 0) {
    warn("Connection closed on read");
    Close(sock);
    return;
  }
  buffer->Append(buf, nread);

  // Parse as many protocol messages as possible
  while (1) {
    int8_t type;
    if (!buffer->Read((char*)&type, 1)) {
      break;
    } 
    switch (type) {
      case REGISTER:
        if (!HandleRegister(sock)) {
          buffer->Unadvance(1);  // tpe
        }
        break;
      case FORWARD:
        if (!HandleForward(sock)) {
          buffer->Unadvance(1);  // type
        }
      default:
        warn("Invalid message type: %d", type);
        Close(sock);
        return;
    }
  }
}

bool PeerConnection::HandleRegister(int sock) {
  Buffer* buffer = GetBuffer(sock);
  assert(buffer);
// TODO(aporter): finish
//  char buf[buffer->Size()];
//  buffer->Read(buf, buffer->Size());
  return false;
}

bool PeerConnection::HandleForward(int sock) {
  Buffer* buffer = GetBuffer(sock);
  assert(buffer);
  return false;
}

void PeerConnection::Close(int sock) {
  select_->RemoveFd(sock);
  close(sock);
  peers_.erase(sock);
}

}  // namespace btunnel
