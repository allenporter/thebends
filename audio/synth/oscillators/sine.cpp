// sine.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/sine.h"

#include <math.h>
#include <cmath>

namespace oscillators {

Sine::Sine() { }

Sine::~Sine() { } 

float Sine::GetValue(float time) {
  return level() * sinf(2.0 * M_PI * frequency() * time);
}

}  // namespace oscillators
