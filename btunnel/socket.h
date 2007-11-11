#ifndef __BTUNNEL_SOCKET_H__
#define __BTUNNEL_SOCKET_H__

#include <sys/types.h>

namespace btunnel {

// Abstract
class Socket {
 public:
  virtual ~Socket();

  int sock();

 protected:
  // Cannot be instantiated directly, see other implementations.
  Socket();

  // Should be set by subclasses;
  int sock_;
};

class SocketBase : public Socket {
 public:
  virtual ~SocketBase();

 protected:
  SocketBase(uint16_t port);

  virtual int create_socket() = 0;

 private:
  void Setup();

  uint16_t port_;
};

}  // namespace btunnel

#endif  // __BTUNNEL_SOCKET_H__
