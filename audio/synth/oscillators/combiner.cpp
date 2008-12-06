// combiner.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/combiner.h"

namespace oscillators {

Combiner::Combiner() { }

Combiner::~Combiner() { }

void Combiner::Clear() {
  oscillators_.clear();
}

void Combiner::Add(Oscillator* oscillator) {
  oscillators_.push_back(oscillator);
}

float Combiner::GetValue(float t) {
  float value = 0.0;
  for (size_t i = 0; i < oscillators_.size(); ++i) {
    value += oscillators_[i]->GetValue(t);
  }
  // Clip
  if (value > 1.0) {
    return 1.0;
  } else if (value < -1.0) {
    return -1.0;
  } 
  return value;
}

}  // namespace oscillators
