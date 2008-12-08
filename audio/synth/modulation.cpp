// modualtion.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/modulation.h"
#include <sys/param.h>
#include "synth/oscillator.h"

namespace synth {

LFO::LFO()
    : amount_(0.0),
      oscillator_(NULL)
      { }

void LFO::set_amount(float amount) {
  amount_ = amount;
}

void LFO::set_oscillator(Oscillator* oscillator) {
  oscillator_ = oscillator;
}

float LFO::GetValue(float t) {
  return 1.0 - amount_ * oscillator_->GetValue(t);
}

}  // namespace synth
