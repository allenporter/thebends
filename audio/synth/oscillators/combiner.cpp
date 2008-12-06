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
    value += level() * oscillators_[i]->GetValue(t);
  }
  // Clip
  float max = level();
  if (value > max) {
    return max;
  } else if (value < (0 - max)) {
    return (0 - max);
  } 
  return value;
}

}  // namespace oscillators
