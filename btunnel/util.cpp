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

void Split(const std::string& message, char c,
           std::vector<std::string>* parts) {
  parts->clear();
  const char *last = message.c_str();
  const char *end = last + message.size();
  const char *cur = message.c_str();
  while (cur < end) {
    if (*cur == c) {
      if ((cur - last) != 0) {
        parts->push_back(std::string(last, cur - last));
      }
      last = cur + 1;
    }
    cur++;
  }
  if (last != end) {
    parts->push_back(std::string(last, cur - last));
  }
}

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
  Split(str, ',', &parts);
  int len = parts.size();
  for (int i = 0; i < len; i++) {
    vector<string> kv;
    Split(parts[i], '=', &kv);
    if (kv.size() != 2) {
      records->clear();
      return false;
    }
    (*records)[kv[0]] = kv[1];
  }
  return true;
}

}  // namespace btunnel
