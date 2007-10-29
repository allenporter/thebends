// reader.cpp
// Author: Allen Porter <allen@thebends.org>

#include "reader.h"
#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sysexits.h>
#include <unistd.h>

namespace ydb {

class ReaderImpl : public Reader {
 public:
  ReaderImpl(int fd, int file_size, int record_size)
    : fd_(fd), file_size_(file_size), record_size_(record_size),
      error_(false), buf_(new char[record_size + 1]) {
  }

  virtual ~ReaderImpl() {
    delete buf_;
  }

  bool ReadRecord(char* buf) {
    if (Eof() || error_) {
      return false;
    }
    int bytes_read;
    if ((bytes_read = read(fd_, buf_, record_size_ + 1)) == -1) {
      warn("read");
      error_ = true;
      return false;
    } else if (bytes_read != record_size_ + 1)  {
      warnx("read: size mismatch");
      error_ = true;
      return false;
    } else if (Checksum(buf_ + 1) != buf_[0]) {
      warnx("checksum mismatch");
      error_ = true;
      return false;
    }
    bytes_read_ += record_size_ + 1;
    memcpy(buf, buf_ + 1, record_size_);
    return true;
  }

  bool Eof() {
    return error_ || bytes_read_ >= file_size_;
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
  int file_size_;
  int bytes_read_;
  int record_size_;
  bool error_;
  char* buf_;
};

Reader* OpenReader(const std::string& file, mode_t mode, int record_size) {
  int fd;
  if ((fd = open(file.c_str(), O_RDONLY, 0)) == -1) {
    warn("open");
    return NULL;
  }
  struct stat st;
  int size;
  if ((size = fstat(fd, &st)) == -1) {
    warn("fstat");
    return NULL;
  }
  return new ReaderImpl(fd, size, record_size);
}

}  // namespace ydb
