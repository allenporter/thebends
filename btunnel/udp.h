#ifndef __BTUNNEL_UDP_H__
#define __BTUNNEL_UDP_H__

#include <sys/types.h>
#include "socket.h"

namespace btunnel {

Socket* NewUdpSocket(uint16_t port);

}  // namespace btunnel

#endif  // __BTUNNEL_UDP_H__
