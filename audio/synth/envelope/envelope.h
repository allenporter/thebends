// envelope.h
// Author: Allen Porter <allen@thebends.org>
//
// An envelope controls a value over time.

#ifndef __ENVELOPE_H__
#define __ENVELOPE_H__

namespace envelope {

class Envelope {
 public:
  Envelope();
  virtual ~Envelope();

  // samples
  void set_attack(int attack);

  // samples
  void set_decay(int decay);

  // Sustain Volumne [0.0, 1.0]
  void set_sustain(float sustain);

  // samples
  void set_release(int release);

  // Invoked when the note is pressed, resets all counters.
  void NoteOn();
  // Invoked when the note is released.
  void NoteOff();

  // Advances the clock and returns the value for the current step.  Should not
  // be called when Done() returns false.
  virtual float GetValue();

 private:
  enum State {
    ATTACK = 0,
    DECAY = 1,
    SUSTAIN = 2,
    RELEASE = 3,
    DONE = 4,
  };
  int attack_;
  float attack_slope_;
  int decay_;
  int decay_end_;
  float decay_slope_;
  float sustain_;
  int release_;
  int release_start_;
  int release_end_;
  float release_slope_;

  int current_;  // sample
  float last_value_;
  State state_;
  float release_start_value_;
};

}  // namespace envelope

#endif  // __ENVELOPE_H__
