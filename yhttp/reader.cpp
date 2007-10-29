#include "reader.h"
#include <err.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace yhttpserver {

Reader::Reader(int sock) : sock_(sock) { }

Reader::~Reader() { }

bool Reader::Read() {
  char rbuf[BUFSIZ];
  int nread;
  if ((nread = read(sock_, rbuf, BUFSIZ)) != 0) {
    printf("Read %d bytes:\n%s\n", nread, rbuf);
    buf_.append(rbuf, nread);
    return true;
  } else if (nread == -1) {
    err(EX_OSERR, "read()");
  }
  return false;
}

}  // namespace httpserver
