// sine.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/sine.h"

#include <math.h>
#include <cmath>

namespace synth {

Sine::Sine() { }

Sine::~Sine() { } 

float Sine::GetValue(float t) {
  return level() * sinf(2.0 * M_PI * frequency() * t);
}

}  // namespace synth
