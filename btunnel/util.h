// util.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_UTIL_H__
#define __BTUNNEL_UTIL_H__

#include <string>
#include <sys/types.h>

namespace btunnel {

// Parses a host:port string into individual components.
bool GetHostPort(const std::string& host_port,
                 std::string* host,
                 uint16_t* port);

}  // namespace btunnel

#endif  // __BTUNNEL_UTIL_H__
