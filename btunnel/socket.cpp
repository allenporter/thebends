#include "socket.h"

namespace btunnel {

Socket::Socket() { }

Socket::~Socket() { }

int Socket::sock() {
  return sock_;
}

}  // namespace btunnel
