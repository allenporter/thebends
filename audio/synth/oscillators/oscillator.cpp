// oscillator.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/oscillator.h"
#include <assert.h>

namespace oscillators {

Oscillator::Oscillator()
    : level_(1.0),
      frequency_(0),
      sample_(0),
      sample_rate_(0) { }

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

int Oscillator::sample_rate() {
  return sample_rate_;
}

void Oscillator::set_sample_rate(int sample_rate) {
  sample_rate_ = sample_rate;
}

void Oscillator::Generate(int num_output_samples,
                          float* output_buffer) {
  for (int i = 0; i < num_output_samples; ++i) {
    sample_ = (sample_ + 1) % sample_rate_;
    output_buffer[i] = GetValue(sample_);
  }
}

}  // namespace oscillators
