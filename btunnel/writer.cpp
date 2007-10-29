// writer.cpp
// Author: Allen Porter <allen@thebends.org>
#include "writer.h"

#include <err.h>
#include <sys/types.h>
#include <sysexits.h>

namespace btunnel {

class SocketWriter : public Writer {
 public:
  SocketWriter(int sock) : sock_(sock) { }
  virtual ~SocketWriter() { }

  virtual void Write(const struct sockaddr_in* dest,
                     const unsigned char* data,
                     size_t length) {
    ssize_t sent = sendto(sock_, data, length, 0 /* flags */,
                          (const struct sockaddr*)dest,
                          sizeof(struct sockaddr_in));
    if ((size_t)sent != length) {
      err(EX_OSERR, "sendto");
    }
  }

 private:
  int sock_;
};

Writer* NewSocketWriter(int sock) {
  return new SocketWriter(sock);
}

}  // namespace btunnel
