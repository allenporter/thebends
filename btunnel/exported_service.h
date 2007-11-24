// exported_service.h
// Author: Allen Porter <allen@thebends.org>
//
// An ExportedService listens on a port for incoming connections, and creates
// Forward requests for a peer object to send across the network.

#ifndef __BTUNNEL_EXPORTED_SERVICE_H__
#define __BTUNNEL_EXPORTED_SERVICE_H__

#include <map>
#include <ythread/callback.h>

namespace ynet {
class BufferedWriter;
class Select;
class TCPServer;
struct Connection;
};

namespace btunnel {

class Peer;
class ForwardRequest;

// Callers should invoke srandom() at the start of the application, before
// creating an ExportedService.
class ExportedService {
 public:
  ExportedService(ynet::Select* select,
                  int port,
                  int service_id,
                  Peer* peer);
  ~ExportedService();

  // Forwards the request to a connected client.  Invoked by the Core when a 
  // message is received from the real remote service.
  bool Forward(const ForwardRequest* request);

 private:
  void Connect(ynet::Connection* conn);
  void Read(int sock);
  void Close(int sock);

  ynet::Select* select_;
  ynet::TCPServer* tcp_server_;
  int port_;
  int service_id_;
  Peer* peer_;

  // Mapping from session ids to sockets and back
  std::map<int, int> session_to_socket_;
  std::map<int, int> socket_to_session_;
  std::map<int, ynet::BufferedWriter*> socket_writers_;
};

}  // namespace btunnel

#endif  // __BTUNNEL_EXPORTED_SERVICE_H__
