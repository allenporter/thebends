// buffer.cpp
// Author: Allen Porter <allen@thebends.org>

#include "buffer.h"

#include <string.h>
#include <iostream>

using namespace std;

namespace btunnel {

Buffer::Buffer(int size) : buffer_size_(size + 1), start_(0), end_(0) {
  assert(Size() == 0);
  buffer_ = new char[buffer_size_];
}

Buffer::~Buffer() {
  delete [] buffer_;
}

int Buffer::Size() const {
  if (start_ == end_) {
    return 0;
  } else if (start_ < end_) {
    return end_ - start_;
  }
  return buffer_size_ - (start_ - end_);
}

int Buffer::SizeLeft() const {
  return buffer_size_ - Size() - 1;
}

bool Buffer::Peek(char* data, int len) const {
  if (len > Size()) {
    return false;
  }
  int start = start_;
  int left = len;
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

bool Buffer::Read(char* data, int len) {
  if (!Peek(data, len)) {
    return false;
  }
  return Advance(len);
}

bool Buffer::Append(const char* data, int len) {
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

bool Buffer::Advance(int len) {
  assert(len > 0);
  if (len > Size()) {
    return false;
  }
  start_ += len;
  start_ %= buffer_size_;
  return true;
}

bool Buffer::Unadvance(int len) {
  assert(len > 0);
  if (len > SizeLeft()) {
    return false;
  }
  start_ -= len;
  start_ %= buffer_size_;
  return true;
}

}  // namespace btunnel
