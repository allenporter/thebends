// util.h
// Author: Allen Porter <allen@thebends.org>

#include <netinet/in.h>

namespace pmp {

// Attempts to get the public ip address and uptime of the gateway.  Returns
// true if successful, and false otherwise.
bool GetGatewayInfo(in_addr* public_ip, uint32_t* uptime);

}  // namespace pmp
