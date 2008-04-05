#include <ythread/callback-inl.h>
#include <err.h>
#include <google/gflags.h>
#include <ynet/select.h>
#include <ynet/util.h>
#include <ynet/tcp_server.h>
#include <ythread/thread.h>
#include <iostream>

using namespace std;

DEFINE_int32(port, 0, "Server port that accepts client requests");
DEFINE_int32(echo_factor, 1, "Number of times to echo each request");

class WorkThread : public ythread::Thread {
 public:
  WorkThread(int sock) : sock_(sock) { }

 protected:
  static const int kBufSize = 255;

  virtual void Run() {
    cout << "Connected" << endl;
    char buf[kBufSize];
    ssize_t nread;
    while ((nread = read(sock_, buf, kBufSize)) != 0) {
      if (nread < 0) {
        perror("read");
        delete this;
        return;
      }
      cout << "Read " << nread << " bytes" << endl;
      for (int i = 0; i < FLAGS_echo_factor; ++i) {
        ssize_t nwrote = write(sock_, buf, nread);
        if (nwrote == 0) {
          break;
        } else if (nwrote < 0) {
          perror("write");
          delete this;
          return;
        }
        cout << "Wrote " << nwrote << " bytes" << endl;
      }
    }
    cout << "Connection closed";
    delete this;
  }

  int sock_;
};


class EchoServer {
 public:
  EchoServer(int port) :
    server_(&select_, port,
            ythread::NewCallback(this, &EchoServer::Connect)) {
  }

  void Start() {
    server_.Start();
    select_.Start();
  }

  void Stop() {
    select_.Stop();
  }

 private:
  void Connect(ynet::Connection* conn) {
    ynet::SetBlocking(conn->sock);
    WorkThread* t = new WorkThread(conn->sock);
    t->Start();
  }

  ynet::Select select_;
  ynet::TCPServer server_;
};

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_port <= 0) {
    errx(1, "Required flag --port was missing");
  }  

  EchoServer server(FLAGS_port);
  server.Start();
}
