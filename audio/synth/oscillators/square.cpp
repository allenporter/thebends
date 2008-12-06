// square.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/square.h"

namespace oscillators {

Square::Square() { }

Square::~Square() { } 

float Square::GetValue(float t) {
  float value = Sine::GetValue(t);
  return (value > 0) ? level() : (0 - level());
}

}  // namespace oscillators
