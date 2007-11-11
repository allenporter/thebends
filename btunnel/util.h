// util.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_UTIL_H__
#define __BTUNNEL_UTIL_H__

#include <string>
#include <sys/types.h>
#include "listener.h"

namespace btunnel {

// Writes contents of received by a Listener to stdout.
class PacketDumper {
 public:
  // The maximum number of bytes displayed from a packet.  Packets greater
  // than max_length are truncated.
  PacketDumper(size_t max_length);
  ~PacketDumper();

  // Callback that can be passed to a listener thread to dump the contents
  // of a packet when invoked.
  ReceivedCallback* callback();

 private:
  void DumpPacket(btunnel::packet_info* info);

  ReceivedCallback* callback_;
  size_t max_length_;
};

// Parses a host:port string into individual components.
bool GetHostPort(const std::string& host_port,
                 std::string* host,
                 uint16_t* port);

}  // namespace btunnel

#endif  // __BTUNNEL_UTIL_H__
