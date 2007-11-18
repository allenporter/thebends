// encoding.cpp
// Author: Allen Porter <allen@thebends.org>

#include "encoding.h"
#include <string>
#include "buffer.h"

using namespace std;

namespace btunnel {

int ReadString(Buffer* buffer, int16_t max_length,  string* input) {
  assert(max_length > 0);
  int nbytes = 0;
  int16_t length;
  if (!buffer->Read((char*)&length, sizeof(int16_t))) {
    // not enough data
    return 0;
  }
  nbytes += sizeof(int16_t);
  length = ntohs(length);
  if (length > max_length) {
    buffer->Unadvance(nbytes);
    return -1;
  }
  char buf[length];
  if (!buffer->Read(buf, length)) {
    // not enough data
    buffer->Unadvance(nbytes);
    return 0;
  }
  nbytes += length;
  *input = string(buf, length);
  return nbytes;
}

int WriteString(Buffer* buffer, int16_t max_length, const string& output) {
  assert(max_length > 0);
  if (output.size() > (size_t)max_length) {
    return -1;
  }
  int nbytes = 0;
  int16_t length = htons(output.size());
  if (!buffer->Append((char*)&length, sizeof(int16_t))) {
    buffer->Unadvance(nbytes);
    return 0;
  }
  nbytes += sizeof(int16_t);
  if (!buffer->Append(output.c_str(), output.size())) {
    return 0;
  }
  nbytes += output.size();
  return nbytes;
}

}  // namespace btunnel
