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
#include <iostream>
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

Core::Core(ynet::Select* select, int sock, vector<btunnel::Service*>* services,
           ShutdownCallback* callback)
      : select_(select), sock_(sock), shutdown_(callback),
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
    cout << "Local service "
         << service->host() << ":" << service->port()
         << " (" << service_id << ")" << endl;
    struct in_addr ip;
    int ret = inet_aton(service->host().c_str(), &ip);
    if (ret != 1) {
      err(1, "inet_aton");
    }
    LocalServiceInfo* info = new LocalServiceInfo;
    info->local = new LocalService(select, service_id, ip, service->port(),
                                   remote_peer_);
    info->service = *iter;
    local_services_[service_id] = info;

    RegisterRequest register_request;
    register_request.service_id = service_id;
    register_request.name = service->name();
    register_request.type = service->type();
    register_request.txt_records = service->txt();
    if (!remote_peer_->Register(-1, &register_request)) {
      errx(1, "Failed to register remote service");
    }
  }
  ynet::SetNonBlocking(sock);
}

Core::~Core() {
  // TODO: Delete exported services
  Shutdown();
}

void Core::Read() {
  cout << "Core: Read data on socket" << endl;
  if (reader_.eof()) {
    // we're done! shut down?
    Shutdown();
    return;
  }
  // may invoke one of the Peer interface methods below
  int nbytes;
  do {
    nbytes = message_reader_.Read(sock_, &reader_);
  } while (nbytes > 0);
}

void Core::Shutdown() {
  cout << "Shutting down core" << endl;
  while (!exported_services_.empty()) {
    map<int, ExportedServiceInfo*>::iterator it = exported_services_.begin();
    ExportedServiceInfo* info = it->second;
    manager_.Unregister(info->service);
    delete info->service;
    delete info->exported;
    delete info;
    exported_services_.erase(it);
  }

  if (shutdown_ != NULL) {
    ShutdownCallback* cb = shutdown_;
    shutdown_ = NULL;
    // caller will probably delete us now
    cb->Execute(sock_);
  }
}

//
// Peer interface, invoked by remote connections
//

bool Core::Register(int sock, const RegisterRequest* request) {
  assert(sock == sock_);
  // Setup a new service
  // Create a new ExportedService, pointed at sock

  int port = random() % 9000 + 9000;
  ExportedServiceInfo* info = new ExportedServiceInfo;
  info->exported = new ExportedService(select_, port, request->service_id,
                                       remote_peer_);
  info->service = new Service(request->name, request->type, "", "", port,
                              request->txt_records);
  manager_.Register(info->service);
  exported_services_[request->service_id] = info;

  cout << "Request to register remote service "
       << request->name << " on port " << info->service->port()
       << " (" << request->service_id << ")" << endl;

   return true;
}

bool Core::Unregister(int sock, const UnregisterRequest* request) {
  assert(sock == sock_);
  if (exported_services_.count(request->service_id) == 0) {
    warn("Request to unregister unknown service %d", request->service_id);
    Shutdown();
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
bool Core::Forward(int sock, const ForwardRequest* request) {
  assert(sock == sock_);
  cout << "Incoming forward " << request->service_id << " "
       << request->session_id << " (" << request->buffer.size() << ")" << endl;
  if (local_services_.count(request->service_id) != 0) {
    LocalServiceInfo* info = local_services_[request->service_id];
    LocalService* service = info->local;
    if (!service->Forward(request)) {
      warnx("Failed to forward to local service %d", request->service_id);
      Shutdown();
      return false;
    }
    return true;
  } else if (exported_services_.count(request->service_id) != 0) {
    ExportedServiceInfo* service = exported_services_[request->service_id];
    if (!service->exported->Forward(request)) {
      warnx("Failed to forward to client for service %d", request->service_id);
      Shutdown();
      return false;
    }
    return true;
  }
  warn("Forward request for unknown service id %d", request->service_id);
  return false;
}

}  // namespace btunnel
