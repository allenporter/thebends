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

  enum WaveType {
    SINE = 0,
    SQUARE = 1,
    TRIANGLE = 2,
    SAWTOOTH = 3,
  };
  void set_wave_type(WaveType wave_type);

  // Amplitude, ranges between 0 and 1
  void set_level(float level);

  // Frequency at which to oscillate
  void set_frequency(int frequency);

  // Returns the value at the specific time [0.0, 1.0].  The returned value
  // returned value is in the range [-1.0, 1.0].
  float GetValue(float t);

 protected:
  float level() { return level_; }

 private:
  WaveType wave_type_;
  float level_;
  int frequency_;
};

}  // namespace synth

#endif  // __OSCILLATOR_H__
