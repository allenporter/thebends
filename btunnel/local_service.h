// local_service.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __BTUNNEL_LOCAL_SERVICE_H__
#define __BTUNNEL_LOCAL_SERVICE_H__

#include <sys/types.h>
#include <netinet/in.h>
#include <map>
#include <ythread/callback.h>

namespace ynet {
class BufferedWriter;
class Select;
};

namespace btunnel {

class Peer;
class ForwardRequest;

// Callers should invoke srandom() at the start of the application, before
// creating a LocalService.
class LocalService {
 public:
  LocalService(ynet::Select* select,
               int service_id,
               struct in_addr addr,
               uint16_t port,
               Peer* peer);
  ~LocalService();

  // Forwards the request to a local server.  Invoked by the Core when a remote
  // client sends data.
  bool Forward(const ForwardRequest* request);

 private:
  void Read(int sock);
  void Close(int sock);

  ynet::Select* select_;
  int service_id_;
  struct sockaddr_in addr_;
  int port_;
  Peer* peer_;

  // Mapping from session ids to sockets and back
  std::map<int, int> session_to_socket_;
  std::map<int, int> socket_to_session_;
  std::map<int, ynet::BufferedWriter*> socket_writers_;
};

}  // namespace btunnel

#endif  // __BTUNNEL_LOCAL_SERVICE_H__
