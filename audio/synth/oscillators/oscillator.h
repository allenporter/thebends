// oscillator.h
// Author: Allen Porter <allen@thebends.org>
//
// An oscillator generates a signal with a particular frequency and amplitude.
// TODO(aporter): Generate() seems like it needs its own abstraction

#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

namespace oscillators {

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

  // Number of samples per second, in Hz.
  int sample_rate();
  void set_sample_rate(int sample_rate);

  // TODO(allen): Move this elsewhere?
  void Generate(int num_output_samples, float* output_buffer);

  // Returns the value at the specific sample (0 <= sample < sample_rate).
  virtual float GetValue(int sample) = 0;

 private:
  float level_;
  int frequency_;
  int sample_;
  int sample_rate_;
};

}  // namespace oscillator

#endif  // __OSCILLATOR_H__
