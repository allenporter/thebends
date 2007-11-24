// buffered_writer.cpp
// Author: Allen Porter <allen@thebends.org>

#include <ythread/callback-inl.h>
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <string>
#include "buffered_writer.h"
#include "select.h"

using namespace std;

namespace ynet {

BufferedWriter::BufferedWriter(Select* select,
                               int fd,
                               int buffer_size,
                               int write_size)
    : select_(select), fd_(fd), buffer_(buffer_size), fd_buffered_(false),
      write_buf_size_(write_size) {
  assert(buffer_size > 0);
  assert(write_size > 0);
  write_buf_ = new char[write_buf_size_];
}

BufferedWriter::~BufferedWriter() {
  delete [] write_buf_;
  if (fd_buffered_) {
    select_->RemoveWriteFd(fd_);
  }
}

bool BufferedWriter::Write(const string& data) {
  return Write(data.data(), data.size());
}

bool BufferedWriter::Write(const char* data, int len) {
  assert(len > 0);
  bool should_buffer = fd_buffered_;
  int nbytes = 0;
  if (!should_buffer && ((nbytes = TryWrite(data, len)) != len)) {
    // Writing would cause this to block, write to the buffer instead and
    // we'll write it later
    should_buffer = true;
  }
  if (should_buffer) {
    if (!fd_buffered_) {
      fd_buffered_ = true;
      select_->AddWriteFd(
          fd_, ythread::NewCallback(this, &BufferedWriter::WriteFromBuffer));
    }
    if (!buffer_.Append(data + nbytes, len - nbytes)) {
      warnx("Append failed, buffer full!");
      return false;
    }
  }
warnx("true!");
  return true;
}

#define min(x, y) (((x) < (y)) ? (x) : (y))

void BufferedWriter::WriteFromBuffer(int fd) {
  assert(fd == fd_);
  assert(fd_buffered_);
  assert(buffer_.Size() > 0);
  int nbytes = 0;
  while (buffer_.Size() > 0) {
    nbytes = min(buffer_.Size(), write_buf_size_);
    // TODO: It would be more efficient to just write directly from the buffer
    // instead of copying it out first.
    if (!buffer_.Read(write_buf_, nbytes)) {
      errx(1, "Unexpected error while reading from buffer");
    }
    int wrote;
    if ((wrote = TryWrite(write_buf_, nbytes)) != nbytes) {
      // Still requires buffering
      buffer_.Unadvance(nbytes - wrote);
      break;
    }
  }
  if (buffer_.Size() == 0) {
    select_->RemoveWriteFd(fd);
    fd_buffered_ = false;
  }
}

int BufferedWriter::TryWrite(const char* data, int len) {
  ssize_t nbytes = write(fd_, data, len);
  if (nbytes == -1) {
    if (errno != EAGAIN) {
      err(1, "write");
    }
    return 0;
  }
  if (nbytes != len) {
    printf("nbytes=%ld, len=%d\n", nbytes, len);
  }
  return nbytes;
}

}  // namespace ynet
