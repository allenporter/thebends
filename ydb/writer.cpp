// writer.cpp
// Author: Allen Porter <allen@thebends.org>

#include "writer.h"
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sysexits.h>
#include <unistd.h>

namespace ydb {

class WriterImpl : public Writer {
 public:
  WriterImpl(int fd, int record_size) : fd_(fd), record_size_(record_size),
                                        error_(false),
                                        buf_(new char[record_size + 1]) { }

  virtual ~WriterImpl() {
    delete buf_;
  }

  bool WriteRecord(char* buf) {
    if (error_) {
      return false;
    }

    // memcpy + write is probably faster than 2 x write?
    char checksum_buffer[record_size_ + 1];
    memcpy(checksum_buffer + 1, buf, record_size_);
    checksum_buffer[0] = Checksum(buf);

    int nbytes = write(fd_, buf, record_size_ + 1);
    if (nbytes == -1) {
      warn("write");
      error_ = true;
      return false;
    } else if (nbytes != record_size_ + 1) {
      warnx("write: size mismatch");
      error_ = true;
      return false;
    } else {
      return true;
    }
  }

  bool Close() {
    if (close(fd_) == -1) {
      warn("close");
      return false;
    } else {
      return !error_;
    }
  }

 protected:
  // Very basic checksum, sum of all characters
  char Checksum(char* buf) {
    char x = 0;
    for (int i = 0; i < record_size_; ++i) {
      x = (x + buf[i]) % 0xff;
    }
    return x;
  }

  int fd_;
  int record_size_;
  bool error_;
  char* buf_;
};

Writer* OpenWriter(const std::string& file, mode_t mode, int record_size) {
  int fd;
  if ((fd = open(file.c_str(), O_WRONLY | O_TRUNC | O_CREAT, mode)) == -1) {
    warn("open");
    return NULL;
  }
  return new WriterImpl(fd, record_size);
}

}  // namespace ydb
