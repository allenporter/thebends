// encoding.h
// Author: Allen Porter <allen@thebends.org>
//
// TODO: Describe Encoding.

#include <string>

namespace btunnel {

class Buffer;

int ReadString(Buffer* buffer, int16_t max_length,  std::string* input);

int WriteString(Buffer* buffer, int16_t max_length, const std::string& output);

}  // namespace btunnel
