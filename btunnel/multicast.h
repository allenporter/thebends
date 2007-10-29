// multicast.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_MULTICAST_H__
#define __BTUNNEL_MULTICAST_H__

#include <sys/types.h>
#include "socket.h"

namespace btunnel {

// Creates a new multicast socket for the speciied multicast group and port.
// The socket unregisters itself from the group on delete.
Socket* NewMulticastSocket(uint32_t multicast_addr, uint16_t port);

}  // namespace btunnel

#endif  // __BTUNNEL_MULTICAST_H__
