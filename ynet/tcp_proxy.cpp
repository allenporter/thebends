// tcpproxy.cpp
// Author: Allen Porter <allen@thebends.org>
//
// Standalone TCP single port proxy, used for basic testing of port forwarding
// routines of btunnel.

#include <arpa/inet.h>
#include <err.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <google/gflags.h>
#include <yhttp/select.h>
#include "tunnel.h"

DEFINE_string(remote_ip, "",
              "");
DEFINE_int32(remote_port, 0,
             "");
DEFINE_int32(local_port, 0,
             "");

int main(int argc, char* argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);

  if (FLAGS_remote_ip.empty()) {
    errx(1, "Required flag --remote_ip was missing");
  } else if (FLAGS_remote_port <= 0) {
    errx(1, "Required flag --remote_port was missing");
  } else if (FLAGS_local_port <= 0) {
    errx(1, "Required flag --local_port was missing");
  }

  struct in_addr remote_addr;
  if (inet_aton(FLAGS_remote_ip.c_str(), &remote_addr) == 0) {
    errx(1, "Invalid --remote_ip specified");    
  }

  yhttpserver::Select select;
  btunnel::Tunnel* tunnel =
    btunnel::NewTcpTunnel(&select, remote_addr,
                          (uint16_t)FLAGS_remote_port,
                          (uint16_t)FLAGS_local_port);
  tunnel->Start();
  select.Start();  // loop forever
}
