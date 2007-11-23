// bproxy.cpp
// Author: Allen Porter <allen@thebends.org>
//
// A bonjour proxy.  Registers a fake bonjour service on a local address that
// is proxied to a remote port/ip.
// TODO(aporter): Retire this tool

#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <google/gflags.h>
#include <ynet/select.h>
#include <ynet/tunnel.h>
#include <iostream>
#include "registration.h"
#include "service.h"
#include "util.h"

DEFINE_int32(local_port, 0,
             "The local port to listen on that --service_type will be "
             "advertised on.");
DEFINE_string(service_name, "",
              "Name of the remote service");
DEFINE_string(service_type, "_daap._tcp",
              "The remote bonjour service type, such as _daap._tcp, "
              "_ssh._tcp.  Only TCP services are currently supported.");
DEFINE_string(remote_ip_port, "",
              "The IP:Port that the remote service is registered on.");
DEFINE_string(txt, "",
              "Text record key=value pairs, comma separated.");

using namespace std;

ynet::Select select_;

void sig_handler(int signal) {
  select_.Stop();
}

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  uint16_t local_port = (uint16_t)FLAGS_local_port;
  string remote_ip;
  uint16_t remote_port;
  if (!btunnel::GetHostPort(FLAGS_remote_ip_port, &remote_ip, &remote_port)) {
    errx(1, "Required flag --remote_ip_port must be specified in the format "
            "<ipaddress>:<port>"); 
  } else if (FLAGS_service_name.empty()) {
    errx(1, "Required flag --name was not specified");
  } else if (FLAGS_service_type.empty()) {
    errx(1, "Required flag --service_type was not specified");
  } else if (local_port <= 0) {
    errx(1, "Required flag --local_port was missing");
  }

  struct in_addr remote_addr;
  if (inet_aton(remote_ip.c_str(), &remote_addr) == 0) {
    errx(1, "Invalid --remote_ip specified");    
  }

  map<string, string> txt_records;
  if (!btunnel::GetMap(FLAGS_txt, &txt_records)) {
    errx(1, "Invalid --txt flag");
  }

  btunnel::Service service(FLAGS_service_name, FLAGS_service_type, "", "",
                           local_port, txt_records);

  btunnel::ServiceManager mgr;
  if (!mgr.Register(&service)) {
    errx(1, "Failed to register service");
  }

  signal(SIGINT, sig_handler);
  signal(SIGKILL, sig_handler);
  signal(SIGTERM, sig_handler);
  signal(SIGQUIT, sig_handler);
  signal(SIGABRT, sig_handler);
  signal(SIGSEGV, sig_handler);
  signal(SIGBUS, sig_handler);

  cout << "Service '" << service.name() << "' "
       << " (" << service.type() << ") registered on port " << local_port
       << endl;
  cout << "Forwarding to " << inet_ntoa(remote_addr) << ":" << remote_port
       << endl;

  ynet::Tunnel* tunnel =
    ynet::NewTcpTunnel(&select_, remote_addr, remote_port,
                       (uint16_t)FLAGS_local_port);
  tunnel->Start();
  select_.Start();  // loop forever

  mgr.Unregister(&service);

  delete tunnel;
}
