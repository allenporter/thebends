// square.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/square.h"

namespace synth {

Square::Square() { }

Square::~Square() { } 

float Square::GetValue(float t) {
  float value = Sine::GetValue(t);
  return (value > 0) ? level() : (0 - level());
}

}  // namespace synth
