// sawtooth.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __SAWTOOTH_H__
#define __SAWTOOTH_H__

#include "synth/oscillator.h"

namespace synth {

class Sawtooth : public Oscillator {
 public:
  Sawtooth();
  virtual ~Sawtooth();

  virtual float GetValue(float t);
};

}  // namespace synth

#endif  // __SAWTOOTH_H__
