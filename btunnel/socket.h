#ifndef __BTUNNEL_SOCKET_H__
#define __BTUNNEL_SOCKET_H__

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

}  // namespace btunnel

#endif  // __BTUNNEL_SOCKET_H__
