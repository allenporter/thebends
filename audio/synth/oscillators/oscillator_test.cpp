// envelope_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <iostream>
#include <vector>
#include "oscillators/sawtooth.h"
#include "oscillators/sine.h"
#include "oscillators/square.h"
#include "oscillators/triangle.h"

#define ASSERT_DOUBLE_EQ(x, y) (assert((x - y) < 0.0000001))

namespace {

static void TestSine() {
  oscillators::Sine osc;
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(0, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(1, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(1));
}

static void TestSquare() {
  oscillators::Square osc;
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(1, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(1));
}

static void TestSawtooth() {
  oscillators::Sawtooth osc;
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(-0.5, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(0.5, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(1, osc.GetValue(1));
}

static void TestTriangle() {
  oscillators::Triangle osc;
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(1.0, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(0.5, osc.GetValue(0.125));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(-0.5, osc.GetValue(0.375));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(-0.5, osc.GetValue(0.625));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(0.5, osc.GetValue(0.875));
  ASSERT_DOUBLE_EQ(1, osc.GetValue(1));
}

static void TestOff() {
  oscillators::Sine osc1;
  oscillators::Square osc2;
  oscillators::Sawtooth osc3;
  oscillators::Triangle osc4;

  std::vector<oscillators::Oscillator*> oscs;
  oscs.push_back(&osc1);
  oscs.push_back(&osc2);
  oscs.push_back(&osc3);
  oscs.push_back(&osc4);

  for (unsigned int i = 0; i < oscs.size(); ++i) {
    oscillators::Oscillator* osc = oscs[i];
    osc->set_level(0.0);
    osc->set_frequency(1.0);
    for (int i = 0; i < 10; ++i) {
      ASSERT_DOUBLE_EQ(0.0, osc->GetValue(i / 10.0));
    }
  }

}

}  // namespace

int main(int argc, char* argv[]) {
  TestSine();
  TestSquare();
  TestSawtooth();
  TestTriangle();
  TestOff();
  std::cout << "PASS" << std::endl;
  return 0;
}
