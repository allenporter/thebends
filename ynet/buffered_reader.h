// buffered_reader.h
// Author: Allen Porter <allen@thebends.org>
//
// Should be used with non-blocking sockets to buffer data read from a socket.
// The callers ready_callback is invoked when this buffer gets data.  The caller
// should check eof() to determine if the connection was closed.

#ifndef __YNET_BUFFERED_READER_H__
#define __YNET_BUFFERED_READER_H__

#include <ythread/callback.h>
#include "buffer.h"

namespace ynet {

class Select;

class BufferedReader : public ReadBuffer {
 public:
  static const int kDefaultReadSize = 4 * 1024;  // 4k

  BufferedReader(Select* select,
                 int fd,
                 ythread::Callback* ready_callback,
                 int buffer_size = kDefaultBufferSize,
                 int read_size = kDefaultReadSize);
  virtual ~BufferedReader();

  virtual bool Peek(char* data, int len) const;
  virtual bool Read(char* data, int len);

  virtual bool Advance(int len);
  virtual bool Unadvance(int len);

  bool eof() { return eof_; }

 private:
  void Ready(int fd);

  Select* select_;
  int fd_;
  ythread::Callback* ready_callback;
  Buffer buffer_;
  char* read_buf_;
  int read_buf_size_;
  int read_size_;
  bool eof_;
};

}  // namespace ynet

#endif  // __YNET_BUFFERED_READER_H__
