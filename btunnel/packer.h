// packer.h
// Author: Allen Porter <allen@thebends.org>
//
// A Packer is used by a tunnel to wrap a message for transit.
#ifndef __BTUNNEL_PACKER_H__
#define __BTUNNEL_PACKER_H__

#include <sys/types.h>

namespace btunnel {

class Packer {
 public:
  virtual ~Packer() { }

  virtual bool Pack(const unsigned char* input,
                    size_t input_length,
                    unsigned char* packed_output,
                    size_t* packed_output_length) = 0;

  virtual bool Unpack(const unsigned char* packed_input,
                      size_t packed_input_length,
                      unsigned char* output,
                      size_t* output_length) = 0;

 protected:
  // Cannot instantiate directly
  Packer() { }
};

Packer* NewIdentityPacker(size_t max_size = 1500);

}  // namespace btunnel

#endif  // __BTUNNEL_PACKER_H__
