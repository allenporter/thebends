// envelope.cpp
// Author: Allen Porter <allen@thebends.org>

#include "envelope/envelope.h"

#include <assert.h>
#include <iostream>

namespace envelope {

Envelope::Envelope() : attack_(1.0),
                       decay_(1.0),
                       sustain_(1.0),
                       release_(1.0),
                       current_(0),
                       state_(DONE) {
}

Envelope::~Envelope() { }

float Envelope::attack() {
  return attack_;
}

void Envelope::set_attack(float attack) {
  assert(attack <= 1.0);
  assert(attack >= 0.0);
  attack_ = attack;
}

float Envelope::decay() {
  return decay_;
}

void Envelope::set_decay(float decay) {
  assert(decay <= 1.0);
  assert(decay >= 0.0);
  decay_ = decay;
}

float Envelope::sustain() {
  return sustain_;
}

void Envelope::set_sustain(float sustain) {
  assert(sustain <= 1.0);
  assert(sustain >= 0.0);
  sustain_ = sustain;
}

float Envelope::release() {
  return release_;
}

void Envelope::set_release(float release) {
  assert(release <= 1.0);
  assert(release >= 0.0);
  release_ = release;
}

void Envelope::NoteOn() {
  current_ = 0;
  decay_start_time_ = 0;
  release_start_time_ = 0;
  state_ = ATTACK;
}

void Envelope::NoteOff() {
  assert(state_ != DONE);
  state_ = RELEASE;
  release_start_time_ = current_; 
}

float Envelope::GetValue() {
  current_++;
  float value = 0;
  switch (state_) {
    case ATTACK:
      value = current_ * attack_;
      if (value >= 1.0) {
        state_ = DECAY;
        decay_start_time_ = current_;
        value = 1.0;
      }
      break;
    case DECAY:
      value = 1.0 - (current_ - decay_start_time_) * decay_;
      if (value < sustain_) {
        if (sustain_ <= 0.0) {
          state_ = DONE;
          value = 0.0;
        } else {
          state_ = SUSTAIN;
          value = sustain_;
        }
      }
      break;
    case SUSTAIN:
      value = sustain_;
      assert(value > 0.0);  // Handled in DECAY
      break;
    case RELEASE:
      value = sustain_ - (current_ - release_start_time_) * release_;
      if (value <= 0.0) {
        state_ = DONE;
        value = 0.0;
      }
      break;
    case DONE:
      value = 0.0;
      break;
    default:
      std::cerr << "Unhandled state: " << state_;
      exit(1);
      break;
  }
  return value;
}


}  // namespace envelope
