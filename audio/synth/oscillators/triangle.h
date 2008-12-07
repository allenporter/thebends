// triangle.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "oscillators/oscillator.h"

namespace oscillators {

class Triangle : public Oscillator {
 public:
  Triangle();
  virtual ~Triangle();

  virtual float GetValue(float t);
};

}  // namespace oscillators

#endif  // __TRIANGLE_H__
