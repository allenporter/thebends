// core.h
// Author: Allen Porter <allen@thebends.org>

#include <vector>
#include <map>
#include <ynet/buffered_reader.h>
#include <ynet/buffered_writer.h>
#include <ythread/callback.h>
#include "peer.h"
#include "registration.h"

namespace ynet {
  class Select;
}

namespace btunnel {

class ExportedService;
class Service;
class LocalService;

struct ExportedServiceInfo {
  ExportedService* exported;
  Service* service;
};

struct LocalServiceInfo {
  LocalService* local;
  Service* service;
};

class Core : public Peer {
 public:
  // TODO: The shutdown interface is kind of strange. Do something better here.
  typedef ythread::Callback1<int> ShutdownCallback;

  Core(ynet::Select* select, int sock,
       std::vector<btunnel::Service*>* services,
       ShutdownCallback* callback);
  virtual ~Core();

  // Peer interface. TODO: Don't subclass Peer, use an inner class instead since
  // nothing external should be calling this.
  virtual bool Register(int sock, const RegisterRequest* request);
  virtual bool Unregister(int sock, const UnregisterRequest* request);
  virtual bool Forward(int sock, const ForwardRequest* request);

  int GetRemoteServiceCount() {
    return exported_services_.size();
  }

  int GetLocalServiceCount() {
    return local_services_.size();
  }

 private:
  void Read();

  void Shutdown();

  ynet::Select* select_;
  int sock_;

  ShutdownCallback* shutdown_;

  // Mappings of local services
  std::map<int, LocalServiceInfo*> local_services_;

  // Fake services, stubs for the remote peer
  std::map<int, ExportedServiceInfo*> exported_services_;

  ynet::BufferedReader reader_;
  MessageReader message_reader_;
  ynet::BufferedWriter writer_;
  Peer* remote_peer_;

  ServiceManager manager_;
};

}  // namespace btunnel
