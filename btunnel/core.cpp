// server.cpp
// Author: Allen Porter <allen@thebends.org>
//
// TODO: Need a way to handle errors and shut down the core service

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ynet/buffered_reader.h>
#include <ynet/buffered_writer.h>
#include <ynet/select.h>
#include <ynet/util.h>
#include <vector>
#include "core.h"
#include "service.h"
#include "peer.h"
#include "peer_message.h"
#include "exported_service.h"
#include "local_service.h"
#include "registration.h"
#include "service.h"

using namespace std;

namespace btunnel {

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
  Core(ynet::Select* select, int sock, vector<btunnel::Service*>* services)
      : select_(select), sock_(sock),
        reader_(select, sock, ythread::NewCallback(this, &Core::Read)),
        message_reader_(this),
        writer_(select, sock),
        remote_peer_(NewPeerWriter(&writer_)) {
    for (vector<btunnel::Service*>::iterator iter = services->begin();
         iter != services->end(); ++iter) {
      // TODO: Less hacky, please, ugh.
      int service_id = random() % btunnel::kMaxServiceId;
      assert(local_services_.count(service_id) == 0);  // oops
      Service* service = (*iter);
      struct in_addr ip;
      int ret = inet_aton(service->host().c_str(), &ip);
      assert(ret == 1);
      LocalServiceInfo* info = new LocalServiceInfo;
      info->local = new LocalService(select, service_id, ip, service->port(),
                                     remote_peer_);
      info->service = *iter;
      local_services_[service_id] = info;
    }
    ynet::SetNonBlocking(sock);
  }

  virtual ~Core() {
    // TODO: Delete exported services
  }

  void Read() {
    if (reader_.eof()) {
      // we're done! shut down? invoke some kind of callback
      return;
    }
    // may invoke one of the Peer interface methods below
    message_reader_.Read(sock_, &reader_);
  }

  //
  // Peer interface, invoked by remote connections
  //

  virtual bool Register(int sock, const RegisterRequest* request) {
    assert(sock == sock_);
    // Setup a new service
    // Create a new ExportedService, pointed at sock

    int32_t service_id;
    std::string name;
    std::string type;
    std::string txt_records; 

    int port = random() % 9000 + 9000;
    ExportedServiceInfo* info = new ExportedServiceInfo;
    info->exported = new ExportedService(select_, port, request->service_id,
                                         remote_peer_);
    info->service = new Service(request->name, request->type, "", "", port,
                                request->txt_records);
    manager_.Register(info->service);
    exported_services_[service_id] = info;
    return true;
  }

  virtual bool Unregister(int sock, const UnregisterRequest* request) {
    assert(sock == sock_);
    if (exported_services_.count(request->service_id) == 0) {
      warn("Request to unregister unknown service %d", request->service_id);
      return false;
    }
    ExportedServiceInfo* info = exported_services_[request->service_id];
    manager_.Unregister(info->service);
    delete info->service;
    delete info->exported;
    delete info;
    exported_services_.erase(request->service_id);
    return true;
  }

  // Invoked by remote clients, either a response to a local service (LAN) or
  // a response from an exported service.
  virtual bool Forward(int sock, const ForwardRequest* request) {
    assert(sock == sock_);
    if (local_services_.count(request->service_id) != 0) {
      LocalServiceInfo* info = local_services_[request->service_id];
      LocalService* service = info->local;
      if (!service->Forward(request)) {
        warn("Failed to forward to local service %d", request->service_id);
        return false;
      }
      return true;
    } else if (exported_services_.count(request->service_id) != 0) {
      ExportedServiceInfo* service = exported_services_[request->service_id];
      if (!service->exported->Forward(request)) {
        warn("Failed to forward to client for service %d", request->service_id);
        return false;
      }
      return true;
    }
    warn("Forward request for unknown service id %d", request->service_id);
    return false;
  }

 private:
  ynet::Select* select_;
  int sock_;

  // Mappings of local services
  map<int, LocalServiceInfo*> local_services_;

  // Fake services, stubs for the remote peer
  map<int, ExportedServiceInfo*> exported_services_;

  ynet::BufferedReader reader_;
  MessageReader message_reader_;
  ynet::BufferedWriter writer_;
  Peer* remote_peer_;

  ServiceManager manager_;
};

}  // namespace btunnel
