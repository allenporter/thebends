// combiner.h
// Author: Allen Porter <allen@thebends.org>
//
// Combines multiple oscillators into a single oscillator.

#ifndef __COMBINER_H__
#define __COMBINER_H__

#include <vector>
#include "oscillators/oscillator.h"

namespace oscillators {

class Combiner {
 public:
  Combiner();
  virtual ~Combiner();

  void Clear();
  void Add(Oscillator* oscillator);

  // Returns the value at the specific sample (0 <= sample < sample_rate).
  virtual float GetValue(int sample) = 0;

 private:
  std::vector<Oscillator*> oscillators_;
};

}  // namespace oscillator

#endif  // __COMBINER_H__
