// sawtooth.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/sawtooth.h"

#include <math.h>
#include <cmath>

namespace oscillators {

Sawtooth::Sawtooth() { }

Sawtooth::~Sawtooth() { } 

float Sawtooth::GetValue(float t) {
  float x = 2.0 * M_PI * frequency() * t;
//  float x = 2.0 * frequency() * t;
  return (2 * x - floor(x));
}

}  // namespace oscillators
