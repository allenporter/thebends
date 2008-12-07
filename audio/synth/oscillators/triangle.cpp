// triangle.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/triangle.h"

#include <math.h>
#include <cmath>

namespace oscillators {

Triangle::Triangle() { }

Triangle::~Triangle() { } 

float Triangle::GetValue(float t) {
  float x = 2.0 * frequency() * t;
  return level() * (2 * fabs(x - 2 * floor(x / 2) - 1) - 1);
}

}  // namespace oscillators
