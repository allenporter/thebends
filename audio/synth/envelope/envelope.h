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

  // State?
  // Events that mark time?

  float attack();
  void set_attack(float attack);

  float decay();
  void set_decay(float decay);

  float sustain();
  void set_sustain(float sustain);

  float release();
  void set_release(float release);

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

  float attack_;
  float decay_;
  float sustain_;
  float release_;
  int current_;
  float last_value_;
  State state_;
  int decay_start_time_;
  int release_start_time_;
  float release_start_value_;
};

}  // namespace envelope

#endif  // __ENVELOPE_H__
