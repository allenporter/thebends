// square.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __SQUARE_H__
#define __SQUARE_H__

#include "oscillators/sine.h"

namespace oscillators {

class Square : public Sine {
 public:
  Square();
  virtual ~Square();

  virtual float GetValue(int sample);
};

}  // namespace oscillators

#endif  // __SQUARE_H__
