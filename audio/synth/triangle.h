// triangle.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "synth/oscillator.h"

namespace synth {

class Triangle : public Oscillator {
 public:
  Triangle();
  virtual ~Triangle();

  virtual float GetValue(float t);
};

}  // namespace synth

#endif  // __TRIANGLE_H__
