// bproxy_client.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Client side of the bonjour proxy.  The client/server version is a replacement
// for the standalone bproxy tool.
//
// The client and server work exactly the same, except that the client connects
// to the server.

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <err.h>
#include <google/gflags.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ynet/select.h>
#include <ynet/tcp_client.h>
#include <iostream>
#include <string>
#include <map>
#include "service.h"
#include "browse.h"
#include "core.h"

DEFINE_string(service_type, "_daap._tcp",
              "The remote bonjour service type, such as _daap._tcp, "
              "_ssh._tcp, etc.  Only TCP services are currently supported.");
DEFINE_string(ip, "",
              "Server ip address");
DEFINE_int32(port, 0,
             "Server port that accepts client requests");

using namespace std;

class BProxyClient {
 public:
  BProxyClient(struct in_addr addr, int port,
               vector<btunnel::Service*>* services)
    : client_(&select_, addr, port,
              ythread::NewCallback(this, &BProxyClient::Connect)),
      services_(services) { }

  void Start() {
    client_.Connect();
    select_.Start();
  }

  void Stop() {
    select_.Stop();
  }

 private:
  void Connect(ynet::Connection* conn) {
    if (conn == NULL) {
      Stop();
      return;
    }
    core_ =
        new btunnel::Core(&select_, conn->sock,  services_,
                          ythread::NewCallback(this, &BProxyClient::Shutdown));
  }

  void Shutdown(int sock) {
    delete core_;
    core_ = NULL;
    Stop();
  }

  ynet::Select select_;
  ynet::TCPClient client_;
  vector<btunnel::Service*>* services_;
  btunnel::Core* core_;
};

static BProxyClient* client_ = NULL;

void sig_handler(int signal) {
  assert(signal == SIGINT);
  client_->Stop();
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_ip.empty()) {
    errx(1, "Required flag --ip was not specified");
  }
  if (FLAGS_port <= 0) {
    errx(1, "Required flag --port was not specified");
  }
  if (FLAGS_service_type.empty()) {
    errx(1, "Required flag --service_type was not specified");
  }
  uint16_t port = (uint16_t)FLAGS_port;
  struct in_addr addr;
  if (inet_aton(FLAGS_ip.c_str(), &addr) != 1) {
    err(1, "Flag --ip was not a valid ip address");
  }

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

  client_ = new BProxyClient(addr, port, &services);
  signal(SIGINT, &sig_handler);
  client_->Start();
  delete client_;
}
