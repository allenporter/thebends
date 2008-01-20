// buffer.cpp
// Author: Allen Porter <allen@thebends.org>

#include "buffer.h"
#include <assert.h>
#include <string.h>
#include <iostream>

using namespace std;

namespace ynet {

Buffer::Buffer(int size) : buffer_size_(size + 1), start_(0), end_(0) {
  assert(Size() == 0);
  buffer_ = new char[buffer_size_];
}

Buffer::~Buffer() {
  delete [] buffer_;
}

int Buffer::Size() const {
  ythread::MutexLock l(&mutex_);  
  return SizeInternal();
}

int Buffer::SizeInternal() const {
  if (start_ == end_) {
    return 0;
  } else if (start_ < end_) {
    return end_ - start_;
  }
  return buffer_size_ - (start_ - end_);
}

int Buffer::SizeLeft() const {
  ythread::MutexLock l(&mutex_);  
  return SizeLeftInternal();
}

int Buffer::SizeLeftInternal() const {
  return buffer_size_ - SizeInternal() - 1;
}

bool Buffer::Peek(char* data, int len) const {
  ythread::MutexLock l(&mutex_);  
  return PeekInternal(data, len);
}

bool Buffer::PeekInternal(char* data, int len) const {
  if (len > SizeInternal()) {
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
  ythread::MutexLock l(&mutex_);  
  if (!PeekInternal(data, len)) {
    return false;
  }
  return AdvanceInternal(len);
}

bool Buffer::Write(const char* data, int len) {
  ythread::MutexLock l(&mutex_);  
  if (len > SizeLeftInternal()) {
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
  ythread::MutexLock l(&mutex_);  
  return AdvanceInternal(len);
}

bool Buffer::AdvanceInternal(int len) {
  assert(len > 0);
  if (len > SizeInternal()) {
    return false;
  }
  start_ += len;
  start_ %= buffer_size_;
  return true;
}

bool Buffer::Unadvance(int len) {
  ythread::MutexLock l(&mutex_);  
  assert(len > 0);
  if (len > SizeLeftInternal()) {
    return false;
  }
  start_ -= len;
  start_ %= buffer_size_;
  return true;
}

}  // namespace ynet
