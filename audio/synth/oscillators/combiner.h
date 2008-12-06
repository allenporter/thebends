// combiner.h
// Author: Allen Porter <allen@thebends.org>
//
// Combines multiple oscillators into a single oscillator.

#ifndef __COMBINER_H__
#define __COMBINER_H__

#include <vector>
#include "oscillators/oscillator.h"

namespace oscillators {

class Combiner : public Oscillator {
 public:
  Combiner();
  virtual ~Combiner();

  void Clear();
  void Add(Oscillator* oscillator);

  virtual float GetValue(float t);

 private:
  std::vector<Oscillator*> oscillators_;
};

}  // namespace oscillator

#endif  // __COMBINER_H__
