// sine.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __SINE_H__
#define __SINE_H__

#include "oscillators/oscillator.h"

namespace oscillators {

class Sine : public Oscillator {
 public:
  Sine();
  virtual ~Sine();

  virtual float GetValue(int sample);
};

}  // namespace oscillators

#endif // __SINE_H__
