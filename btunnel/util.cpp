// util.cpp
// Author: Allen Porter <allen@thebends.org>
#include "util.h"

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

namespace btunnel {

bool GetHostPort(const string& host_port, string* host, uint16_t* port) {
  size_t c = host_port.find(":");
  if (c == string::npos) {
    return false;
  }
  *host = host_port.substr(0, c);
  *port = (uint16_t)strtoll(host_port.substr(c + 1).c_str(), NULL, 10);
  return (!host->empty() || *port > 0);
}


}  // namespace btunnel
