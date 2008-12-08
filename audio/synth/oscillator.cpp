// oscillator.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/oscillator.h"
#include <assert.h>
#include <math.h>
#include <cmath>

namespace synth {

Oscillator::Oscillator()
    : wave_type_(SINE),
      level_(1.0),
      frequency_(0) { }

Oscillator::~Oscillator() { }

void Oscillator::set_wave_type(WaveType wave_type) {
  wave_type_ = wave_type;
} 

void Oscillator::set_level(float level) {
  assert(level_ >= 0.0);
  assert(level_ <= 1.0);
  level_ = level;
}

void Oscillator::set_frequency(int frequency) {
  assert(frequency >= 0);
  frequency_ = frequency;
}

float Oscillator::GetValue(float t) {
  float x = frequency_ * t;
  float value = 0;
  switch (wave_type_) {
    case SINE:
    case SQUARE:
      value = sinf(2.0 * M_PI * x);
      if (wave_type_ == SQUARE) {
        value = (value > 0) ? 1.0 : -1.0;
      }
      break;
    case TRIANGLE:
      value = (2 * fabs(2.0 * x - 2 * floor(x) - 1) - 1);
      break;
    case SAWTOOTH:
      value = 2 * (x - floor(x) - 0.5);
      break;
    default:
      assert(false);
      break;
  }
  return level_ * value;
}

}  // namespace synth
