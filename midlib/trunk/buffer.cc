#include "buffer.h"

#include <iostream>
#include <string.h>
#include <strings.h>

using namespace std;

namespace midi {

Buffer::Buffer(const char* data, unsigned long len)
  : data_(data), len_(len), pos_(0) {
}

Buffer::~Buffer() { }

bool Buffer::Read(char* c) {
  return Read(c, 1);
}

bool Buffer::Read(unsigned char* c) {
  return Read((char*)c, 1);
}

bool Buffer::Read(char* buf, unsigned long count) {
  if (pos_ + count > len_) {
    cerr << "Read past end of buffer! (" << pos_ + count << " > "
         << len_ << ")" << endl;
    return false;
  }
  bcopy(&data_[pos_], buf, count);
  pos_ += count; 
  return true;
}

bool Buffer::Read(unsigned short* value) {
  if (pos_ + sizeof(unsigned short) > len_) {
    cerr << "Read past end of buffer!" << endl;
    return false;
  }
  bcopy(&data_[pos_], value, sizeof(unsigned short));
  pos_ += sizeof(unsigned short);
  return true;
}

bool Buffer::Read(unsigned long* value) {
  if (pos_ + sizeof(unsigned long) > len_) {
    cerr << "Read past end of buffer!" << endl;
    return false;
  }
  bcopy(&data_[pos_], value, sizeof(unsigned long));
  pos_ += sizeof(unsigned long);
  return true;
}

bool Buffer::ReadVar(unsigned int* value) {
  unsigned int v = 0;
  int numcount = 0;
  *value = 0;
  while (numcount <= 4) {
    unsigned char c = 0;
    if (!Read((char*)&c)) {
      return false;
    }
    if ((c & 0x80) == 0x80) {
      v = (v<<7) + c - 0x80;
    } else {
      v = (v<<7) + c;
      break;
    }
    numcount++;
  }
  *value = v;
  return true;
}

bool Buffer::Seek(unsigned long offset) {
  if (offset > len_) {
    cerr << "Read past end of buffer!" << endl;
    return false;
  }
  pos_ = offset;
  return true;
}

bool Buffer::SeekRelative(unsigned long offset) {
  return Seek(pos_ + offset);
}

}  // namespace midi
