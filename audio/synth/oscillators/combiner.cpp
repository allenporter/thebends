// combiner.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/combiner.h"

namespace oscillators {

Combiner::Combiner() { }

Combiner::~Combiner() { }

float Combiner::GetValue(int sample) {
  float value = 0.0;
  for (size_t i = 0; i < oscillators_.size(); ++i) {
    value += oscillators_[i]->GetValue(sample);
  }
  if (value > 1.0) {
    return 1.0;
  } else if (value < -1.0) {
    return -1.0;
  } 
  return value;
}

}  // namespace oscillators
