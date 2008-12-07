// sawtooth.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __SAWTOOTH_H__
#define __SAWTOOTH_H__

#include "oscillators/oscillator.h"

namespace oscillators {

class Sawtooth : public Oscillator {
 public:
  Sawtooth();
  virtual ~Sawtooth();

  virtual float GetValue(float t);
};

}  // namespace oscillators

#endif  // __SAWTOOTH_H__
