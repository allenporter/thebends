// envelope_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "synth/oscillator.h"

#define ASSERT_DOUBLE_EQ(x, y) (assert(fabs(x - y) < 0.000001))

namespace {

static void TestSine() {
  synth::Oscillator osc;
  osc.set_wave_type(synth::Oscillator::SINE);
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(0.0, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(1.0, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(0.0, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(-1.0, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(0.0, osc.GetValue(1));
}

static void TestSquare() {
  synth::Oscillator osc;
  osc.set_wave_type(synth::Oscillator::SQUARE);
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(1, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(1, osc.GetValue(1));
}

static void TestSawtooth() {
  synth::Oscillator osc;
  osc.set_wave_type(synth::Oscillator::SAWTOOTH);
  osc.set_level(1.0);
  osc.set_frequency(1.0);  // one cycle per second

  ASSERT_DOUBLE_EQ(-1, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(-0.75, osc.GetValue(0.125));
  ASSERT_DOUBLE_EQ(-0.5, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(-0.25, osc.GetValue(0.375));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(0.25, osc.GetValue(0.625));
  ASSERT_DOUBLE_EQ(0.5, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(0.75, osc.GetValue(0.875));
  ASSERT_DOUBLE_EQ(-1, osc.GetValue(1));
}

static void TestSawtooth2() {
  synth::Oscillator osc;
  osc.set_wave_type(synth::Oscillator::SAWTOOTH);
  osc.set_level(1.0);
  osc.set_frequency(2.0);  // two cycles per second

  ASSERT_DOUBLE_EQ(-1.0, osc.GetValue(0));
  ASSERT_DOUBLE_EQ(-0.5, osc.GetValue(0.125));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.25));
  ASSERT_DOUBLE_EQ(0.5, osc.GetValue(0.375));
  ASSERT_DOUBLE_EQ(-1.0, osc.GetValue(0.5));
  ASSERT_DOUBLE_EQ(-0.5, osc.GetValue(0.625));
  ASSERT_DOUBLE_EQ(0, osc.GetValue(0.75));
  ASSERT_DOUBLE_EQ(0.5, osc.GetValue(0.875));
  ASSERT_DOUBLE_EQ(-1.0, osc.GetValue(1));
}

static void TestTriangle() {
  synth::Oscillator osc;
  osc.set_wave_type(synth::Oscillator::TRIANGLE);
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
  synth::Oscillator osc1;
  osc1.set_wave_type(synth::Oscillator::SINE);
  synth::Oscillator osc2;
  osc2.set_wave_type(synth::Oscillator::SQUARE);
  synth::Oscillator osc3;
  osc3.set_wave_type(synth::Oscillator::TRIANGLE);
  synth::Oscillator osc4;
  osc4.set_wave_type(synth::Oscillator::SAWTOOTH);

  std::vector<synth::Oscillator*> oscs;
  oscs.push_back(&osc1);
  oscs.push_back(&osc2);
  oscs.push_back(&osc3);
  oscs.push_back(&osc4);

  for (unsigned int i = 0; i < oscs.size(); ++i) {
    synth::Oscillator* osc = oscs[i];
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
  TestSawtooth2();
  TestTriangle();
  TestOff();
  std::cout << "PASS" << std::endl;
  return 0;
}
