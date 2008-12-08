// oscillator.h
// Author: Allen Porter <allen@thebends.org>
//
// An oscillator generates a signal with a particular frequency and amplitude.

#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

namespace synth {

class Oscillator {
 public:
  Oscillator();
  virtual ~Oscillator();

  // Amplitude, ranges between 0 and 1
  float level();
  void set_level(float level);

  // Frequency at which to oscillate
  int frequency();
  void set_frequency(int frequency);

  // Returns the value at the specific time [0.0, 1.0].  The returned value
  // returned value is in the range [-1.0, 1.0].
  virtual float GetValue(float t) = 0;

 private:
  float level_;
  int frequency_;
};

}  // namespace synth

#endif  // __OSCILLATOR_H__
