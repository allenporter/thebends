#include <assert.h>
#include <iostream>
#include "multicast.h"

using namespace std;

#define INADDR_MULTICAST_BONJOUR 0xEE0000FB
#define MDNS_PORT 9090

class DummyHandler {
 public:
  DummyHandler(bool should_invoke)
    : should_invoke_(should_invoke),
      invoked_(false) { }

  ~DummyHandler() {
    assert(should_invoke_ && invoked_ ||
           !should_invoke_ && !invoked_);
  } 

  void Invoke(struct btunnel::packet_info* info) {
    invoked_ = true;
  }

 private:
  bool should_invoke_;
  bool invoked_;
};

static void Test1() {
  DummyHandler handler(false);
  ythread::Thread* t =
    btunnel::NewMulticastListenerThread(
      INADDR_MULTICAST_BONJOUR, MDNS_PORT,
      ythread::NewCallback(&handler, &DummyHandler::Invoke));
  t->Start();
  t->Stop();
  t->Join();
  delete t;
}

int main(int argc, char* argv[]) {
  Test1();
  cout << "PASS" << endl;
}
