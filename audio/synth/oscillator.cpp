// oscillator.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/oscillator.h"
#include <assert.h>

namespace synth {

Oscillator::Oscillator()
    : level_(1.0),
      frequency_(0) { }

Oscillator::~Oscillator() { }

float Oscillator::level() {
  return level_;
}

void Oscillator::set_level(float level) {
  assert(level_ >= 0.0);
  assert(level_ <= 1.0);
  level_ = level;
}

int Oscillator::frequency() {
  return frequency_;
}

void Oscillator::set_frequency(int frequency) {
  frequency_ = frequency;
}

}  // namespace synth
