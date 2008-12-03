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
  void Start();
  // Invoked when the note is released.  GetValue() may be called until Done()
  // returns true.
  void Release();

  // Returns true when the note is done playing.
  bool Done();

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
  State state_;
  int decay_start_time_;
  int release_start_time_;
};

}  // namespace envelope

#endif  // __ENVELOPE_H__
