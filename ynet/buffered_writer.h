// buffered_writer.h
// Author: Allen Porter <allen@thebends.org>
//
// BufferedWriter is a class that manages writing to a socket/file descriptor
// and bufferes data when the fd would block.  BufferedWriter uses a Select
// object to recieve notifications when the buffer is available for writing
// again.  BufferedWriter should be used with non-blocking sockets.

#ifndef __YNET_BUFFERED_WRITER_H__
#define __YNET_BUFFERED_WRITER_H__

#include <string>
#include "buffer.h"

namespace ynet {

class Select;

class BufferedWriter : public WriteBuffer {
 public:
  static const int kDefaultWriteSize = 4 * 1024;  // 4k

  BufferedWriter(Select* select,
                 int fd,
                 int buffer_size = kDefaultBufferSize,
                 int write_size = kDefaultWriteSize);
  ~BufferedWriter();

  // Writes the specified data, or queues it for writing later if writing it
  // now would block.
  bool Write(const char* data, int len);

  bool Buffered() const { return fd_buffered_; }
  
 private:
  void WriteFromBuffer(int fd);
  int TryWrite(const char* data, int len);

  Select* select_;
  int fd_;
  Buffer buffer_;
  bool fd_buffered_;
  char* write_buf_;
  int write_buf_size_;
};

}  // namespace ynet

#endif  // __YNET_BUFFERED_WRITER_H__
