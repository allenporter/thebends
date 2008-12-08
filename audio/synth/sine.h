// sine.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __SINE_H__
#define __SINE_H__

#include "synth/oscillator.h"

namespace synth {

class Sine : public Oscillator {
 public:
  Sine();
  virtual ~Sine();

  virtual float GetValue(float t);
};

}  // namespace synth

#endif // __SINE_H__
