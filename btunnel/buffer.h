// buffer.h
// Author: Allen Porter <allen@thebends.org>
//
// Fixed length circular buffer.

#include <sys/types.h>

namespace btunnel {

static const int kDefaultBufferSize = 1024 * 1024;  // 1MB

// Buffer is not threadsafe.
class Buffer {
 public:
  Buffer(int buffer_size = kDefaultBufferSize);
  ~Buffer();

  // Size of data in the buffer.
  int Size() const;
  int SizeLeft() const;

  bool Peek(char* data, int len) const;
  bool Read(char* data, int len);

  bool Advance(int len);
  bool Unadvance(int len);

  // Appends the specified data into the buffer.
  bool Append(const char* data, int len);

 private:
  char* buffer_;
  int buffer_size_;
  int start_;
  int end_; 
};

}  // namespace btunnel
