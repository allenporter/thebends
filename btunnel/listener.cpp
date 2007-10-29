// listener.cpp
// Author: Allen Porter <allen@thebends.org>
#include "listener.h"

#include <assert.h>
#include <err.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sysexits.h>
#include <strings.h>
#include <unistd.h>
#include <ythread/mutex.h>
#include <iostream>

namespace btunnel {

class Listener : public ythread::Thread {
 public:
  Listener(int sock, ReceivedCallback* cb)
    : sock_(sock), cb_(cb), shutdown_(false) {
  }

  virtual ~Listener() {
    assert(shutdown_);
  }

  // L < lock_
  virtual void Stop() {
    lock_.Lock();
    shutdown_ = true;
    lock_.Unlock();
  }

 protected:
  // L < lock_
  virtual void Run() {
    // We set a timeout here only to check that this thread has been asked to
    // shut down.
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 10 * 1000;  // 10 ms
    fd_set rfds;
    lock_.Lock();
    while (!shutdown_) {
      lock_.Unlock();
      FD_ZERO(&rfds);
      FD_SET(sock_, &rfds);
      select(sock_ + 1, &rfds, NULL, NULL, &timeout);
      if (FD_ISSET(sock_, &rfds)) {
        HandleIncomingPacket();
      }
      lock_.Lock();
    } 
    lock_.Unlock();
  }

  // L >= lock_
  void HandleIncomingPacket() {
    struct packet_info info;
    bzero(info.data, kPacketBufferSize);
    u_int sin_len = sizeof(struct sockaddr_in);
    ssize_t nread;
    if ((nread = recvfrom(sock_, info.data, kPacketBufferSize, 0,
                          (struct sockaddr *)&info.sender,
                          &sin_len)) == -1) {
      err(EX_OSERR, "recvfrom");
    }
    assert(nread > 0);
    assert(sin_len == sizeof(struct sockaddr_in));
    info.sender.sin_port = ntohs(info.sender.sin_port);
    info.length = (size_t)nread;
    cb_->Execute(&info);
  }

  // Readonly after constructor
  int sock_;
  ReceivedCallback* cb_;

  ythread::Mutex lock_;  // protects members below
  bool shutdown_;
};

ythread::Thread* NewSocketListenerThread(int sock, ReceivedCallback* cb) {
  return new Listener(sock, cb);
}

}  // namespace btunnel
