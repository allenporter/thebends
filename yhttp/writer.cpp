#include "writer.h"
#include <err.h>
#include <sysexits.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

namespace yhttpserver {

Writer::Writer(int sock) : sock_(sock) { }

Writer::~Writer() { }

void Writer::WriteString(const std::string& message) {
  buf_.append(message);
}

void Writer::WriteString(const char* message, size_t size) {
  buf_.append(message, size);
}

bool Writer::Flush() {
  // TODO: Async write?
  ssize_t nwrote = write(sock_, buf_.c_str(), buf_.size());
  if (nwrote != 0) {
    printf("Wrote %ld bytes:\n%s\n", nwrote, buf_.c_str());
    buf_.clear(); 
    return true;
  } else if (nwrote == -1) {
    err(EX_OSERR, "write()");
  }
  return false;
}

}  // namespace yhttpserver
