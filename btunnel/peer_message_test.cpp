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
    invoked_ = false;
    type_ = 0;
  }

  virtual bool Register(int sock, const btunnel::RegisterRequest* request) {
    assert(!invoked_);
    assert(type_ == 0);
    invoked_ = true;
    type_ = btunnel::REGISTER;
    return false;
  }

  virtual bool Unregister(int sock, const btunnel::UnregisterRequest* request) {
    assert(!invoked_);
    assert(type_ == 0);
    invoked_ = true;
    type_ = btunnel::UNREGISTER;
    return false;
  }

  virtual bool Forward(int sock, const btunnel::ForwardRequest* request) {
    assert(!invoked_);
    assert(type_ == 0);
    invoked_ = true;
    type_ = btunnel::FORWARD;
    return false;
  }

  bool invoked_;
  int type_;
};

static void RandomRegisterRequest(btunnel::RegisterRequest* request) {
  request->service_id = random() % btunnel::kMaxServiceId;
  request->name = RandString(btunnel::kMaxNameLen);
  request->type = RandString(btunnel::kMaxTypeLen);
  request->txt_records = RandString(btunnel::kMaxTextLen);
}

static void RandomUnregisterRequest(btunnel::UnregisterRequest* request) {
  request->service_id = random() % btunnel::kMaxServiceId;
}

static void RandomForwardRequest(btunnel::ForwardRequest* request) {
  request->service_id = random() % btunnel::kMaxServiceId;
  request->buffer = RandString(random() % btunnel::kMaxBufLen);
}

static void test1() {
  btunnel::RegisterRequest request;
  btunnel::Buffer buffer;
  assert(0 == btunnel::ReadRegister(&buffer, &request));
}

static void test2() {
  btunnel::Buffer buffer;
  for (int i = 0; i < 10; ++i) {
    btunnel::RegisterRequest request1;
    RandomRegisterRequest(&request1);

    int ret = WriteRegister(&buffer, request1);
    assert(ret > 0);
    assert(ret == buffer.Size());

    btunnel::RegisterRequest request2;
    assert(ret == ReadRegister(&buffer, &request2));
    assert(ret > 0);
    assert(buffer.Size() == 0);

    assert(request1.service_id == request2.service_id);
    assert(request1.name == request2.name);
    assert(request1.type == request2.type);
    assert(request1.txt_records == request2.txt_records);
  }
}

static void test3() {
  btunnel::Buffer buffer;
  for (int i = 0; i < 10; ++i) {
    btunnel::UnregisterRequest request1;
    RandomUnregisterRequest(&request1);

    int ret = WriteUnregister(&buffer, request1);
    assert(ret > 0);
    assert(ret == buffer.Size());

    btunnel::UnregisterRequest request2;
    assert(ret == ReadUnregister(&buffer, &request2));
    assert(ret > 0);
    assert(buffer.Size() == 0);

    assert(request1.service_id == request2.service_id);
  }
}

static void test4() {
  btunnel::Buffer buffer;
  for (int i = 0; i < 10; ++i) {
    btunnel::ForwardRequest request1;
    RandomForwardRequest(&request1);

    int ret = WriteForward(&buffer, request1);
    assert(ret > 0);
    assert(ret == buffer.Size());

    btunnel::ForwardRequest request2;
    assert(ret == ReadForward(&buffer, &request2));
    assert(ret > 0);
    assert(buffer.Size() == 0);

    assert(request1.service_id == request2.service_id);
    assert(request1.buffer == request2.buffer);
  }
}

static void test5() {
  FakePeer peer;
  btunnel::Buffer buffer;
  btunnel::MessageReader reader(&peer);
  int sock = random() % 1024 + 1024;

  assert(!peer.invoked_);
  reader.Read(sock, &buffer);
  assert(!peer.invoked_);

  for (int i = 0; i < 10; i++) {
    peer.Reset();
    {
      btunnel::RegisterRequest request;
      RandomRegisterRequest(&request);
      buffer.Append("\x10", 1);  // type = REGISTER
      WriteRegister(&buffer, request);

      assert(!peer.invoked_);
      reader.Read(sock, &buffer);
      assert(peer.invoked_);
      assert(peer.type_ == btunnel::REGISTER);
    }
    peer.Reset();
    {
      btunnel::UnregisterRequest request;
      RandomUnregisterRequest(&request);
      buffer.Append("\x11", 1);  // type = UNREGISTER
      WriteUnregister(&buffer, request);

      assert(!peer.invoked_);
      reader.Read(sock, &buffer);
      assert(peer.invoked_);
      assert(peer.type_ == btunnel::UNREGISTER);
    }
    peer.Reset();
    {
      btunnel::ForwardRequest request;
      RandomForwardRequest(&request);
      buffer.Append("\x20", 1);  // type = FORWARD
      WriteForward(&buffer, request);

      assert(!peer.invoked_);
      reader.Read(sock, &buffer);
      assert(peer.invoked_);
      assert(peer.type_ == btunnel::FORWARD);
    }
  }
}

int main(int argc, char* argv) {
  srandom(time(NULL));
  test1();
  test2();
  test3();
  test4();
  test5();
  cout << "PASS" << endl;
}
