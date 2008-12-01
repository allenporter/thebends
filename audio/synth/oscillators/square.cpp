// square.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/square.h"

namespace oscillators {

Square::Square() { }

Square::~Square() { } 

float Square::GetValue(int sample) {
  float value = Sine::GetValue(sample);
  return (value > 0) ? 1.0 : -1.0;
}

}  // namespace oscillators
