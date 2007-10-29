#ifndef __BUFFER_H__
#define __BUFFER_H__

namespace midi {

class Buffer {
 public:
  // Data must exist for the lifetime of the buffer
  Buffer(const char* data, unsigned long length);
  virtual ~Buffer();

  // Reads a single byte into c.
  bool Read(char* c);
  bool Read(unsigned char* c);

  // Reads count bytes into buf.  buf must already have enough space to
  // hold count bytes.
  bool Read(char* buf, unsigned long count);

  // Reads an unsigned short into value
  bool Read(unsigned short* value);

  // Reads an unsigned long into value
  bool Read(unsigned long* value);

  // Reads a variable length integer
  bool ReadVar(unsigned int* value);

  // Seeks to the specified offset
  bool Seek(unsigned long offset);

  // Seeks to the specified offset relative to the current position
  bool SeekRelative(unsigned long offset);

  bool Eof() const { return pos_ == len_; }

  char Peek() const { return data_[pos_]; }

  unsigned long Position() const { return pos_; }

  unsigned long Length() const { return len_; };

  unsigned long BytesLeft() const { return (len_ - pos_); }

  const char* data_;
 private:
  unsigned long len_;
  unsigned long pos_;
};

}  // namespace midi

#endif
