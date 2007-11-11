// util.cpp
// Author: Allen Porter <allen@thebends.org>
#include "util.h"

#include <ythread/callback-inl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include "listener.h"

using namespace std;

namespace btunnel {

PacketDumper::PacketDumper(size_t max_length) : max_length_(max_length) {
  callback_ = ythread::NewCallback(this, &PacketDumper::DumpPacket);
}

PacketDumper::~PacketDumper() {
  delete callback_;
}

ReceivedCallback* PacketDumper::callback() {
  return callback_;
}

void PacketDumper::DumpPacket(btunnel::packet_info* info) {
  cout << "Received from "
       << inet_ntoa(info->sender.sin_addr)
       << ":" << info->sender.sin_port << endl;
  int num = (info->length > max_length_) ? max_length_
                                         : info->length;
  for (int i = 0; i < num; ++i) {
    printf(" %02x", info->data[i]);
  }
  if (info->length > max_length_) {
    cout << " ...";
  }
  cout << endl;
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


}  // namespace btunnel
