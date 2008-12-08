// modulation.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __MODULATION_H__
#define __MODULATION_H__

namespace synth {

class Oscillator;

class LFO {
 public:
  LFO();

  void set_oscillator(Oscillator* oscillator);
  void set_amount(float amount);

  float GetValue(float t);

 private:
  float amount_;
  Oscillator* oscillator_;
};

}  // namespace synth

#endif  // __MODULATION_H__
