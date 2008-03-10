#include <ythread/callback-inl.h>
#include <err.h>
#include <errno.h>
#include <google/gflags.h>
#include <ynet/select.h>
#include <ynet/util.h>
#include <ynet/tcp_client.h>
#include <ythread/thread.h>
#include <iostream>

using namespace std;

DEFINE_int32(port, 0, "Server port that accepts client requests");
DEFINE_int32(message_size, 512, "Size of each message");

class Client {
 public:
  Client(in_addr addr, int16_t port) :
    client_(&select_, addr, port,
            ythread::NewCallback(this, &Client::Connected)) {
  }

  bool ClearReadBuffer() {
    char buf[255];
    while (true) {
      ssize_t nread = read(sock_, buf, 255);
      if (nread == 0) {
        return false;
      } if (nread == -1 && errno == EAGAIN) {
        return true;
      } else if (nread < 0) {
        perror("read");
        return false;
      }
    }
  }

  void Start() {
    cout << "Connecting..." << endl;
    client_.Connect();
    select_.Start();
    assert(sock_ > 0);
    char buf[FLAGS_message_size];
    bzero(buf, FLAGS_message_size);
    int i = 0;
    while (true) {
      i++;
      if (!ClearReadBuffer()) {
        break;
      }
      ssize_t nwrote = write(sock_, buf, FLAGS_message_size);
      if (nwrote == 0) {
        break;
      } else if (nwrote == -1 && errno == EAGAIN) {
        usleep(100);
        cout << "Throttle" << endl;
        continue;
      }
      cout << "Write " << i << endl;
      if (nwrote < 0) {
        perror("write");
        break;
      } else if (nwrote != FLAGS_message_size) {
        errx(1, "Write wrong number of bytes (%d)", nwrote);
      }
    }
    cout << "End of client" << endl;
    exit(1);
  }

 protected:
  void Connected(ynet::Connection* conn) {
    if (conn == NULL) {
      exit(0);
    }
    sock_ = conn->sock;
    cout << "Connected on sock(" << sock_ << ")" << endl;
    select_.Stop();
  }

  ynet::Select select_;
  ynet::TCPClient client_;
  int sock_;
};

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_port <= 0) {
    errx(1, "Required flag --port was missing");
  }
  struct in_addr addr;
  int ret = inet_aton("127.0.0.1", &addr);
  if (ret != 1) {
    errx(1, "Can't get address for 127.0.0.1");
  }
  Client c(addr, FLAGS_port);
  c.Start();
  cout << "End" << endl;

}
