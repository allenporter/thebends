// bproxy_server.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Server side bonjour proxy.  The client/server version is a replacement for
// the standalone bproxy tool.
//
// The server side exports the first service found that matches --service_type.
// A client and server communicate over a backchannel that forwards the details
// of the locally registered service so that it can be registered on the
// clients computer.

#include <ythread/callback-inl.h>
#include <err.h>
#include <google/gflags.h>
#include <ynet/select.h>
#include <ynet/tcp_server.h>
#include <iostream>
#include <string>
#include <map>
#include "service.h"
#include "browse.h"
#include "core.h"

DEFINE_string(service_type, "_daap._tcp",
              "The remote bonjour service type, such as _daap._tcp, "
              "_ssh._tcp, etc.  Only TCP services are currently supported.");
DEFINE_int32(port, 0,
             "Server port that accepts client requests");

using namespace std;


class BProxyServer {
 public:
  BProxyServer(int port, vector<btunnel::Service*>* services)
    : server_(&select_, port,
              ythread::NewCallback(this, &BProxyServer::Connect)),
      services_(services) { }

  void Start() {
    server_.Start();
    select_.Start();
  }

  void Stop() {
    select_.Stop();
  }

 private:
  void Connect(ynet::Connection* conn) {
    sock_map_[conn->sock] =
        new btunnel::Core(&select_, conn->sock,  services_,
                          ythread::NewCallback(this, &BProxyServer::Shutdown));
  }

  void Shutdown(int sock) {
    map<int, btunnel::Core*>::iterator it = sock_map_.find(sock);
    assert(it != sock_map_.end());
    delete sock_map_[sock];
    sock_map_.erase(it);
  }

  ynet::Select select_;
  ynet::TCPServer server_;
  vector<btunnel::Service*>* services_;
  map<int, btunnel::Core*> sock_map_;
};

static BProxyServer* server_ = NULL;

void sig_handler(int signal) {
  assert(signal == SIGINT);
  server_->Stop();
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_port <= 0) {
    errx(1, "Required flag --port was not specified");
  }
  if (FLAGS_service_type.empty()) {
    errx(1, "Required flag --service_type was not specified");
  }
  uint16_t port = (uint16_t)FLAGS_port;
  cout << "Looking for services of type '" << FLAGS_service_type << "'" << endl;

  vector<btunnel::Service*> services;
  btunnel::BrowseService(FLAGS_service_type, &services);

  cout << "Total services found: " << services.size() << endl;
  for (unsigned int i = 0; i < services.size(); ++i) {
    btunnel::Service* service = services[i];
    cout << "Found: " << service->name() << " "
         << "(" << service->type() << ") at "
         << service->host() << ":" << service->port()
         << "; " << service->domain() << endl;
    const map<string, string>& records = service->txt_records();
    for (map<string, string>::const_iterator it = records.begin();
         it != records.end(); ++it) {
      cout << "  " << it->first << " => " << it->second << endl;
    }
  }

  server_ = new BProxyServer(port, &services);
  signal(SIGINT, &sig_handler);
  server_->Start();
  delete server_;
}
