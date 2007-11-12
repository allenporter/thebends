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
  Buffer(size_t buffer_size = kDefaultBufferSize);
  ~Buffer();

  // Size of data in the buffer.
  size_t Size() const;
  size_t SizeLeft() const;

  bool Peek(unsigned char* data, size_t len) const;

  bool Read(unsigned char* data, size_t len);

  bool Advance(size_t len);

  bool Append(const unsigned char* data, size_t len);

 private:
  unsigned char* buffer_;
  size_t buffer_size_;
  size_t start_;
  size_t end_; 
};

}  // namespace btunnel
