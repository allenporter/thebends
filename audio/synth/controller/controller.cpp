// controller.cpp
// Author: Allen Porter <allen@thebends.org>

#include "oscillators/sine.h"

namespace {



}

int main(int argc, char* argv[]) {
  oscillators::Sine sin;
  sin.set_level(1.0);
  sin.set_frequency(440);
  sin.set_sample_rate(44000);

  return 0;
}
