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
  return value;
}

}  // namespace oscillators
