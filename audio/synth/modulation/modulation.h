// modulation.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __MODULATION_H__
#define __MODULATION_H__

namespace oscillators { class Oscillator; }

namespace modulation {

class LFO {
 public:
  LFO();

  void set_oscillator(oscillators::Oscillator* oscillator);
  void set_amount(float amount);

  float GetValue(float t);

 private:
  float amount_;
  oscillators::Oscillator* oscillator_;
};

}  // namespace modulation

#endif  // __MODULATION_H__
