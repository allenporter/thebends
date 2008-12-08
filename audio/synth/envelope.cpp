// synth.cpp
// Author: Allen Porter <allen@thebends.org>

#include "synth/envelope.h"

#include <assert.h>
#include <iostream>

namespace synth {

Envelope::Envelope() : attack_(0),
                       attack_slope_(0.0),
                       decay_(0),
                       decay_end_(0),
                       decay_slope_(0.0),
                       sustain_(1.0),
                       release_(0),
                       release_start_(0),
                       release_end_(0),
                       release_slope_(0.0),
                       current_(0),
                       state_(DONE) {
}

Envelope::~Envelope() { }

void Envelope::set_attack(long attack) {
  assert(attack >= 0);
  attack_ = attack;
}

void Envelope::set_decay(long decay) {
  assert(decay >= 0);
  decay_ = decay;
}

void Envelope::set_sustain(float sustain) {
  assert(sustain <= 1.0);
  assert(sustain >= 0.0);
  sustain_ = sustain;
}

void Envelope::set_release(long release) {
  assert(release >= 0);
  release_ = release;
}

void Envelope::NoteOn() {
  current_ = 0;
  decay_end_ = attack_ + decay_;
  attack_slope_ = 1.0 / attack_;
  decay_slope_ = (1.0 - sustain_) / decay_;
  state_ = ATTACK;
}

void Envelope::NoteOff() {
  state_ = RELEASE;
  release_slope_ = sustain_ / release_;
  release_start_value_ = std::min(sustain_, last_value_);
  release_start_ = current_; 
  release_end_ = current_ + release_;
}

float Envelope::GetValue() {
  current_++;
  float value = 0;

  // Check that we haven't transitioned longo the next state
  if (state_ == ATTACK || state_ == DECAY) {
    if (current_ > decay_end_) {
      state_ = SUSTAIN;
    } else if (current_ > attack_) {
      state_ = DECAY;
    }
  }
  if (state_ == SUSTAIN) {
    if (sustain_ <= 0.0) {
      state_ = DONE;
    }
  }
  if (state_ == RELEASE) {
    if (current_ > release_end_) {
      state_ = DONE;
    }
  }

  switch (state_) {
    case ATTACK:
      value = current_ * attack_slope_;
      break;
    case DECAY:
      assert(current_ >= attack_);
      value = 1.0 - (current_ - attack_) * decay_slope_;
      value = std::max(value, sustain_);
      break;
    case SUSTAIN:
      value = sustain_;
      assert(value > 0.0);  // Handled in DECAY
      break;
    case RELEASE:
      assert(current_ >= release_start_);
      value = release_start_value_ -
           (current_ - release_start_) * release_slope_;
      break;
    case DONE:
      value = 0.0;
      break;
    default:
      std::cerr << "Unhandled state: " << state_;
      exit(1);
      break;
  }
  last_value_ = value;
  return value;
}

}  // namespace synth
