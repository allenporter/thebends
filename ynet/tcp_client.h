// tcp_client.h
// Auhor: Allen Porter <allen@thebends.org>
//
// Basic TCP Client implementation.  The client connects to the specified host
// and port and invokes the callers callback when a new connection is made.

#ifndef __YNET_TCP_CLIENT_H__
#define __YNET_TCP_CLIENT_H__

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

class TCPClient {
 public:
  // Client callback is invoked when the server connects.  The callback
  // is owned by the TCPClient and will take care of deallocating it.
  TCPClient(Select* select,
            struct in_addr remote_addr,
            int16_t remote_port,
            ConnectionCallback* callback);
  ~TCPClient();

  void Connect();

  bool Connected() { return connected_; }

 private:
  void Ready(int sock);
  void Stop();

  ynet::Select* select_;
  struct in_addr addr_;
  int16_t port_;
  int sock_;
  ConnectionCallback* client_callback_;
  bool connected_;
};

}  // namespace ynet

#endif  // __YNET_TCP_CLIENT_H__
