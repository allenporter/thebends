// tcp_server.h
// Auhor: Allen Porter <allen@thebends.org>
//
// Basic TCP Server implementation.  This server listens for connections on
// the specified port, and invokes a callers callback when a new connection is
// made.

#ifndef __YNET_TCP_SERVER_H__
#define __YNET_TCP_SERVER_H__

#include <netinet/in.h>
#include <sys/types.h>
#include <ythread/callback.h>

namespace ynet {

class Select;

struct Connection {
  struct sockaddr_in addr;
  int sock;
};

typedef ythread::Callback1<Connection*> ConnectionCallback;

class TCPServer {
 public:

  // Client callback is invoked whenever a new connection arrives.  The callback
  // is owned by the TCPServer and will take care of deallocating it.
  TCPServer(Select* select,
            int16_t local_port,
            ConnectionCallback* callback);
  ~TCPServer();

  void Start();
  void Stop();

 private:
  void Accept(int sock);

  ynet::Select* select_;
  int16_t port_;
  int sock_;
  ConnectionCallback* client_callback_;
};

}  // namespace ynet

#endif  // __YNET_TCP_SERVER_H__
