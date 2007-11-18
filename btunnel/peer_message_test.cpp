// peer_message_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <string>
#include <iostream>
#include "buffer.h"
#include "peer.h"
#include "peer_message.h"
#include "test_util.h"

using namespace std;

class FakePeer : public btunnel::Peer {
 public:
  FakePeer() {
    Reset();
  }

  virtual ~FakePeer() { }

  void Reset() {
    type_ = 0;
  }

  virtual bool Register(int sock, const btunnel::RegisterRequest* request) {
    assert(type_ == 0);
    type_ = btunnel::REGISTER;
    return false;
  }

  virtual bool Unregister(int sock, const btunnel::UnregisterRequest* request) {
    assert(type_ == 0);
    type_ = btunnel::UNREGISTER;
    return false;
  }

  virtual bool Forward(int sock, const btunnel::ForwardRequest* request) {
    assert(type_ == 0);
    type_ = btunnel::FORWARD;
    return false;
  }

  int type_;
};

static void test1() {
  btunnel::RegisterRequest request;
  btunnel::Buffer buffer;
  assert(0 == btunnel::ReadRegister(&buffer, &request));
}

static void test2() {
  btunnel::Buffer buffer;

  for (int i = 0; i < 10; ++i) {
    btunnel::RegisterRequest request1;
    request1.service_id = random() % 0xffffffff;
    request1.name = RandString(btunnel::kMaxNameLen);
    request1.type = RandString(btunnel::kMaxTypeLen);
    request1.txt_records = RandString(btunnel::kMaxTextLen);

    int ret = WriteRegister(&buffer, request1);
    assert(ret == buffer.Size());
    btunnel::RegisterRequest request2;
    assert(ret == ReadRegister(&buffer, &request2));
    assert(buffer.Size() == 0);
  }
}

int main(int argc, char* argv) {
  srandom(time(NULL));
  test1();
  test2();
  cout << "PASS" << endl;
}
