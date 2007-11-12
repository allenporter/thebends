// buffer.cpp
// Author: Allen Porter <allen@thebends.org>

#include "buffer.h"

#include <string.h>
#include <iostream>

using namespace std;

namespace btunnel {

Buffer::Buffer(size_t size) : buffer_size_(size + 1), start_(0), end_(0) {
  assert(Size() == 0);
  buffer_ = new unsigned char[buffer_size_];
}

Buffer::~Buffer() {
  delete [] buffer_;
}

size_t Buffer::Size() const {
  if (start_ == end_) {
    return 0;
  } else if (start_ < end_) {
    return end_ - start_;
  }
  return buffer_size_ - (start_ - end_);
}

size_t Buffer::SizeLeft() const {
  return buffer_size_ - Size() - 1;
}

bool Buffer::Peek(unsigned char* data, size_t len) const {
  if (len > Size()) {
    return false;
  }
  size_t start = start_;
  size_t left = len;
  if (start_ + len > buffer_size_) {
    size_t forward = buffer_size_ - start_;
    memcpy(data, buffer_ + start, forward);
    start = 0;
    left -= forward;
  }
  if (left > 0) {
    memcpy(data + (len - left), buffer_ + start, left);
  }
  return true;
}

bool Buffer::Read(unsigned char* data, size_t len) {
  if (!Peek(data, len)) {
    return false;
  }
  return Advance(len);
}

bool Buffer::Append(const unsigned char* data, size_t len) {
  if (len > SizeLeft()) {
    return false;
  }
  size_t left = len;
  if (end_ + len > buffer_size_) {
    size_t forward = buffer_size_ - end_;
    memcpy(buffer_ + end_, data, forward);
    left -= forward;
    end_ = 0; //(end_ + forward) % buffer_size_;
  }
  if (left > 0) {
    memcpy(buffer_ + end_, data + (len - left), left);
    end_ = (end_ + left) % buffer_size_;
  }
  return true;
}

bool Buffer::Advance(size_t len) {
  if (len > Size()) {
    return false;
  }
  start_ = (start_ + len) % buffer_size_;
  return true;
}

}  // namespace btunnel
