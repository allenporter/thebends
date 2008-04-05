// exported_service_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <arpa/inet.h>
#include <err.h>
#include <stdlib.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ynet/select.h>
#include <iostream>
#include "exported_service.h"
#include "peer.h"
#include "peer_message.h"
#include "test_util.h"

using namespace std;

class FakePeer : public btunnel::Peer {
 public:
  FakePeer(ynet::Select* loop) : loop_(loop) {
    invoked_ = false;
  }

  void Reset() {
    invoked_ = false;
    request_.service_id = 0;
    request_.session_id = 0;
    request_.buffer.clear();
  }

  bool Forward(int sock, const btunnel::ForwardRequest* request) {
    assert(!invoked_);
    invoked_ = true;
    request_.service_id = request->service_id;
    request_.session_id = request->session_id;
    request_.buffer = request->buffer;
    loop_->Stop();
    return true;
  }

  ynet::Select* loop_;
  bool invoked_;
  btunnel::ForwardRequest request_;
};

static int LocalConnect(int16_t port) {
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  int ret = inet_aton("127.0.0.1", &addr.sin_addr);
  assert(ret == 1);
  addr.sin_port = htons(port);
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    err(1, "sock");
  }
  ret = connect(sock, (const struct sockaddr*)&addr,
                sizeof(struct sockaddr_in));
  if (ret != 0) {
    err(1, "connect");
  }
  return sock;
}

static void test1() {
  int port = test::RandomPort();
  int service_id = random() % 1024;

  ynet::Select loop;
  FakePeer peer(&loop);
  btunnel::ExportedService service(&loop, port, service_id, &peer); 

  int sock = LocalConnect(port);
  assert(sock > 0);

  assert(write(sock, "ABCDEFG", 7) == 7);
  loop.Start();

  assert(peer.invoked_);
  assert(peer.request_.service_id == service_id);
  assert(peer.request_.session_id > 0);

  // same session id is used for all requests
  int session_id = peer.request_.session_id;
  for (int i = 0; i < 10; i++) {
    peer.Reset();
    assert(write(sock, "A", 1) == 1);
    assert(!peer.invoked_);
    loop.Start();
    assert(peer.invoked_);
    assert(peer.request_.service_id == service_id);
    assert(peer.request_.session_id == session_id);
  }

  close(sock);
}

static void test2() {
  int port = test::RandomPort();
  int service_id = random() % 1024;

  ynet::Select loop;
  FakePeer peer(&loop);
  btunnel::ExportedService service(&loop, port, service_id, &peer); 
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  test1();
  test2();
  cout << "PASS" << endl;
}
