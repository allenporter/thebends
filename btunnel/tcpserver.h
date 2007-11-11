// tcpserver.h
// Auhor: Allen Porter <allen@thebends.org>

#include <netinet/in.h>
#include <sys/types.h>
#include <ythread/callback.h>
#include <yhttp/select.h>

namespace btunnel {

struct Connection {
  struct sockaddr_in addr;
  int sock;
};

typedef ythread::Callback1<Connection*> ConnectionCallback;

class TCPServer {
 public:

  // Client callback is invoked whenever a new connection arrives.  The callback
  // is owned by the TCPServer and will take care of deallocating it.
  TCPServer(yhttpserver::Select* select,
            uint16_t local_port,
            ConnectionCallback* callback);
  ~TCPServer();

  void Start();
  void Stop();

 private:
  void Accept(int sock);

  yhttpserver::Select* select_;
  uint16_t port_;
  int sock_;
  ConnectionCallback* client_callback_;
};

}  // namespace btunnel
