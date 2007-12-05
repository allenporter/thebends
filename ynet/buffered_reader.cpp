// buffered_reader.cpp
// Author: Allen Porter <allen@thebend.org>

#include <ythread/callback-inl.h>
#include <errno.h>
#include <err.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include "buffered_reader.h"
#include "select.h"

namespace ynet {

BufferedReader::BufferedReader(Select* select,
                               int fd,
                               ythread::Callback* ready_callback,
                               int buffer_size,
                               int read_size)
    : select_(select), fd_(fd), ready_callback_(ready_callback),
      buffer_(buffer_size), read_buf_size_(read_size), eof_(false) {
  assert(buffer_size > 0);
  assert(read_size > 0);
  read_buf_ = new char[read_buf_size_];
  select_->AddReadFd(fd_, ythread::NewCallback(this, &BufferedReader::Ready));
}

BufferedReader::~BufferedReader() {
  delete [] read_buf_;
  select_->RemoveReadFd(fd_);
}

bool BufferedReader::Peek(char* data, int len) const {
  return buffer_.Peek(data, len);
} 

bool BufferedReader::Read(char* data, int len) {
  return buffer_.Read(data, len);
} 

bool BufferedReader::Advance(int len) {
  return buffer_.Advance(len);
}
 
bool BufferedReader::Unadvance(int len) {
  return buffer_.Unadvance(len);
} 

void BufferedReader::Ready(int fd) {
  assert(fd_ == fd);
  bool changed = false;
  int nbytes;
  do {
    nbytes = read(fd, read_buf_, read_buf_size_);
    if (nbytes == -1) {
      if (errno != EAGAIN) {
        err(1, "read");
      }
      break;
    } else if (nbytes == 0) {
      eof_ = true;
      changed = true;
      break;
    }
    buffer_.Write(read_buf_, nbytes);
    changed = true;
  } while (true);
  if (changed) {
    ready_callback_->Execute();
  }
}

}  // namespace ynet
