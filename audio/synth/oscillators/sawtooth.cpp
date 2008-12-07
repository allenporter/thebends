// sawtooth.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/sawtooth.h"

#include <math.h>
#include <cmath>

namespace oscillators {

Sawtooth::Sawtooth() { }

Sawtooth::~Sawtooth() { } 

float Sawtooth::GetValue(float t) {
  float x = frequency() * t;
  return level() * 2 * (x - floor(x) - 0.5);
}

}  // namespace oscillators
