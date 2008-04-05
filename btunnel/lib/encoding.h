// encoding.h
// Author: Allen Porter <allen@thebends.org>
//
// TODO: Describe Encoding.

#include <string>

namespace ynet {
class ReadBuffer;
class WriteBuffer;
}

namespace btunnel {

int ReadString(ynet::ReadBuffer* buffer,
               int16_t max_length,
               std::string* input);

int WriteString(ynet::WriteBuffer* buffer,
                int16_t max_length,
                const std::string& output);

}  // namespace btunnel
