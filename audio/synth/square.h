// square.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __SQUARE_H__
#define __SQUARE_H__

#include "synth/sine.h"

namespace synth {

class Square : public Sine {
 public:
  Square();
  virtual ~Square();

  virtual float GetValue(float t);
};

}  // namespace synth

#endif  // __SQUARE_H__
