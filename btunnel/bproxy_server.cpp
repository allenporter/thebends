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

#include <err.h>
#include <google/gflags.h>
#include <yhttp/select.h>
#include <iostream>
#include <string>
#include <map>
#include "service.h"
#include "browse.h"

DEFINE_string(service_type, "_daap._tcp",
              "The remote bonjour service type, such as _daap._tcp, "
              "_ssh._tcp, etc.  Only TCP services are currently supported.");
DEFINE_int32(port, 0,
             "Server port that accepts client requests");

using namespace std;

yhttpserver::Select select_;

void sig_handler(int signal) {
  select_.Stop();
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

//  uint16_t port = (uint16_t)FLAGS_port;
  if (FLAGS_service_type.empty()) {
    errx(1, "Required flag --service_type was not specified");
  }
  cout << "Looking for services of type '" << FLAGS_service_type << "'" << endl;

  vector<btunnel::Service*> services;
  btunnel::BrowseService(FLAGS_service_type, &services);

  cout << "Total services found: " << services.size() << endl;
  for (unsigned int i = 0; i < services.size(); ++i) {
    btunnel::Service* service = services[i];
    cout << "Found: " << service->name() << " "
         << "(" << service->type() << ") at "
         << service->host() << ":" << service->port() << endl;
    const map<string, string>& records = service->txt_records();
    for (map<string, string>::const_iterator it = records.begin();
         it != records.end(); ++it) {
      cout << "  " << it->first << " => " << it->second << endl;
    }
  }

  cout << "Picking the first server, to make life easier." << endl;
}
