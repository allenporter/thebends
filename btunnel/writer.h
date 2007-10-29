// writer.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __BTUNNEL_WRITER_H__
#define __BTUNNEL_WRITER_H__

#include <netinet/in.h>
#include <sys/types.h>

namespace btunnel {

// Abstract interface for writing packets.  See NewSocketWriter().
class Writer {
 public:
  virtual ~Writer() { }

  virtual void Write(const struct sockaddr_in* dest,
                     const unsigned char* data,
                     size_t length) = 0;
 protected:
  // Cannot be instantiated directly
  Writer() { }
};

// Creates a Writer that can write packets to the specified socket.
Writer* NewSocketWriter(int sock);

}  // namespace btunnel

#endif  // __BTUNNEL_WRITER_H__
