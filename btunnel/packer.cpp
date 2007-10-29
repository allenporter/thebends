// packer.cpp
// Author: Allen Porter <allen@thebends.org>
#include "packer.h"

#include <string.h>

namespace btunnel {

class PackerImpl : public Packer {
 public:
  PackerImpl(size_t max_size) : max_size_(max_size) { }

  virtual ~PackerImpl() { }

  virtual bool Pack(const unsigned char* input,
                    size_t input_length,
                    unsigned char* packed_output,
                    size_t* packed_output_length) {
    if (input_length > max_size_) {
      return false;
    }
    *packed_output_length = input_length;
    memcpy(packed_output, input, input_length);
    return true;
  }

  virtual bool Unpack(const unsigned char* packed_input,
                      size_t packed_input_length,
                      unsigned char* output,
                      size_t* output_length) {
    return Pack(packed_input, packed_input_length, output, output_length);
  }

 protected:
  size_t max_size_;
};

Packer* NewIdentityPacker(size_t max_size) {
  return new PackerImpl(max_size);
}

}  // namespace btunnel
