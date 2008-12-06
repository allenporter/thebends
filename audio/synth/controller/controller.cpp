// controller.cpp
// Author: Allen Porter <allen@thebends.org>

#include "controller/controller.h"

#include <math.h>
#include <assert.h>
#include "envelope/envelope.h"
#include "oscillators/oscillator.h"

namespace controller {

// Use A above Middle C as the reference frequency
static const float kMiddleAFrequency = 440.0;
static const int kMiddleANote = 69;
static const float kNotesPerOctave = 12.0;

// Get the MIDI note frequency based on the 
// TODO(allen): Keep this around to set the frequency of the envelope
//static float GetFrequencyForNote(int note) {
//  return kMiddleAFrequency * powf(2, (note - kMiddleANote) / kNotesPerOctave);
//}

Controller::Controller(int sample_rate)
    : sample_rate_(sample_rate),
      sample_(0),
      volume_(1.0),
      oscillator_(NULL),
      volume_envelope_(NULL) { }

void Controller::set_volume(float volume) {
  assert(volume >= 0.0);
  assert(volume <= 1.0);
  volume_ = volume;
}

float Controller::volume() {
  return volume_;
}

void Controller::GetSamples(int num_output_samples, float* output_buffer) {
  assert(oscillator_);
  assert(volume_envelope_);

  for (int i = 0; i < num_output_samples; ++i) {
    sample_ = (sample_ + 1) % sample_rate_;
    float t = sample_ / (float)sample_rate_;
    float amplitude =
        volume_ * volume_envelope_->GetValue() * oscillator_->GetValue(t);
    output_buffer[i] = amplitude;
  }
}

}  // namespace controller
