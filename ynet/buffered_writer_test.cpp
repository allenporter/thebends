// buffered_writer_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <assert.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ythread/condvar.h>
#include <ythread/mutex.h>
#include <ythread/thread.h>
#include <iostream>
#include <string>
#include "buffered_writer.h"
#include "tcp_server.h"
#include "select.h"
#include "util.h"

using namespace std;

class FakeServer : public ythread::Thread {
 public:
  FakeServer(int16_t port) : port_(port),
                             condvar_(new ythread::CondVar(&mutex_)) {
    Start();
    ythread::MutexLock l(&mutex_);
    condvar_->Wait();
  }

  virtual ~FakeServer() {
    select_.Stop();
    Join();
  }

  void Run() {
    ynet::TCPServer server(&select_, port_,
                           ythread::NewCallback(this, &FakeServer::Connect));
    server.Start();

    mutex_.Lock();
    condvar_->Signal();
    mutex_.Unlock();

    select_.Start();  // Loop!
  }

  void Connect(ynet::Connection* conn) {
    // Do nothing
  }

 private:
  ynet::Select select_;
  int16_t port_;

  ythread::Mutex mutex_;
  ythread::CondVar* condvar_;
};

static int RandomPort() {
  return random() % 1024 + 9000;
}

void test1() {
  int port = RandomPort();
  FakeServer server(port);

  struct sockaddr_in addr = ynet::SockAddr("127.0.0.1", port);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  assert(sock > 0);

  int ret = connect(sock, (struct sockaddr*)&addr, sizeof(addr));
  if (ret == -1) {
    err(1, "connect");
  }
  assert(ret == 0);
  ynet::SetNonBlocking(sock);

  int size = 256 * 1024;
  char* buf = new char[size];
  bzero(buf, size);
  {
    // size is too large for the socket, and too large for the buffer
    ynet::Select select;
    ynet::BufferedWriter writer(&select, sock, 512, 256);
    assert(!writer.Write(buf, size));
    assert(!writer.Buffered());
  }
  {
    // size is too large for the socket, but not the buffer
    ynet::Select select;
    ynet::BufferedWriter writer(&select, sock, size * 2, 256);
    assert(writer.Write(buf, size));
    assert(writer.Buffered());
  }
  delete [] buf;
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  test1();
  cout << "PASS" << endl;
}
