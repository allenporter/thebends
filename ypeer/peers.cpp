// peers.cpp
// Author: Allen Porter <allen@thebends.org>

#include "peers.h"
#include <iostream>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <yhttp/httpserver.h>
#include <ynet/select.h>
#include <ynet/tcp_client.h>
#include <ythread/callback-inl.h>
#include <ythread/condvar.h>
#include <ythread/mutex.h>
#include <ythread/thread.h>

using namespace std;

namespace ypeer {

class CBThread : public ythread::Thread {
 public:
  CBThread(ythread::Callback* cb) : cb_(cb) { }

 protected:
  virtual void Run() {
    cb_->Execute();
  }

 private:
  ythread::Callback* cb_;
};

class PeersImpl : public Peers {
 public:
  in_addr addr;
  PeersImpl(in_addr addr, int16_t port, const string& path, const string& room)
      : path_(path),
        room_(room),
        client_(&select_, addr, port,
                ythread::NewCallback(this, &PeersImpl::Connected)),
        thread_(ythread::NewCallback(this, &PeersImpl::Work)),
        cond_(&mu_),
        connected_(false),
        sock_(0) {
  }

  ~PeersImpl() {
    select_.Stop();
    thread_.Join();
  }

  virtual bool Connect() {
    thread_.Start();
    mu_.Lock();
    cout << "Wait..." << endl;
    cond_.Wait();
    bool result = connected_;
    mu_.Unlock();
    return result;
  }

  virtual bool GetPeers(vector<Peer>* peers) {
    return false;
  }

 private:
  void Work() {
    client_.Connect();
    select_.Start();
  }

  void Connected(ynet::Connection* conn) {
    mu_.Lock();
    connected_ = (conn != NULL);
    if (connected_) {
      sock_ = conn->sock;
    }
    cond_.Signal();
    mu_.Unlock();
    mu_.Unlock();
  }

  string path_;
  string room_;
  ynet::Select select_;
  ynet::TCPClient client_;
  CBThread thread_;
  ythread::Mutex mu_;
  ythread::CondVar cond_;
  bool connected_;
  int sock_;
};

Peers* NewPeers(const std::string& tracker_url, const std::string& room) {
  yhttpserver::URL url(tracker_url);
  if (!url.IsValid()) {
    return NULL;
  }
  assert(url.User().empty());
  assert(url.Password().empty());
  assert(url.Scheme() == "http");
  assert(url.Query().empty());
  hostent* hp = gethostbyname(url.Host().c_str());
  if (hp == NULL) {
    cerr << "Unable to resolve hostname " << url.Host() << endl;
    return NULL;
  }
  in_addr addr;
  memcpy(&addr, hp->h_addr, sizeof(struct in_addr));
  return new PeersImpl(addr, url.Port(), url.Path(), room);
}

}  // namespace ypeer
