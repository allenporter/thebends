// buffer.h
// Author: Allen Porter <allen@thebends.org>
//
// Dynamic length circular buffer.

#ifndef __YNET_BUFFER_H__
#define __YNET_BUFFER_H__

#include <sys/types.h>
#include <ythread/mutex.h>

namespace ynet {

static const int kDefaultBufferSize = 1024 * 1024;  // 1MB

class ReadBuffer {
 public:
  virtual ~ReadBuffer() { }

  virtual bool Peek(char* data, int len) const = 0;
  virtual bool Read(char* data, int len) = 0;

  virtual bool Advance(int len) = 0;
  virtual bool Unadvance(int len) = 0;

 protected:
  ReadBuffer() { }
};

class WriteBuffer {
 public:
  virtual ~WriteBuffer() { }

  virtual bool Write(const char* data, int len) = 0;

 protected:
  WriteBuffer() { }
};

// Buffer is threadsafe
class Buffer : public WriteBuffer, public ReadBuffer {
 public:
  Buffer(int buffer_size = kDefaultBufferSize);
  virtual ~Buffer();

  // Size of data in the buffer.
  int Size() const;
  int SizeLeft() const;

  bool Peek(char* data, int len) const;
  bool Read(char* data, int len);

  bool Advance(int len);
  bool Unadvance(int len);

  // Appends the specified data into the buffer.
  bool Write(const char* data, int len);

 private:
  // Must be called with mutex_ held
  bool PeekInternal(char* data, int len) const;
  int SizeInternal() const;
  int SizeLeftInternal() const;
  bool AdvanceInternal(int len);
  void Grow();

  mutable ythread::Mutex mutex_;
  char* buffer_;
  int buffer_size_;
  int start_;
  int end_; 
};

}  // namespace ynet

#endif  // __YNET_BUFFER_H__
