// util.cpp
// Author: Allen Porter <allen@thebends.org>
#include "util.h"

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <vector>
#include <yhttp/util.h>

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

bool GetMap(const string& str, map<string, string>* records) {
  records->clear();
  vector<string> parts;
  yhttpserver::Split(str, ',', &parts);
  int len = parts.size();
  for (int i = 0; i < len; i++) {
    vector<string> kv;
    yhttpserver::Split(parts[i], '=', &kv);
    if (kv.size() != 2) {
      records->clear();
      return false;
    }
    (*records)[kv[0]] = kv[1];
  }
  return true;
}

}  // namespace btunnel
