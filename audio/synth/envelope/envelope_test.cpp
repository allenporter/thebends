// envelope_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <math.h>
#include <iostream>
#include "envelope/envelope.h"

#define ASSERT_DOUBLE_EQ(x, y) (assert(fabs(x - y) < 0.0000001))

namespace {

static void TestFlat() {
  envelope::Envelope env;
  env.NoteOn();
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(1.0, env.GetValue());
  }
  env.NoteOff();
  assert(0.0 == env.GetValue());
}

static void TestZero() {
  envelope::Envelope env;
  env.set_attack(0);
  env.set_decay(0);
  env.set_sustain(0);
  env.set_release(0);
  env.NoteOn();
  for (int i = 0; i < 10; ++i) { 
    ASSERT_DOUBLE_EQ(0.0, env.GetValue());
  }
  env.NoteOff();
  ASSERT_DOUBLE_EQ(0.0, env.GetValue());
}

static void TestCurve() {
  envelope::Envelope env;
  env.set_attack(4);
  env.set_decay(4);
  env.set_sustain(0.45);
  env.set_release(8);
  env.NoteOn();
  // Attack
  ASSERT_DOUBLE_EQ(0.25, env.GetValue());
  ASSERT_DOUBLE_EQ(0.5, env.GetValue());
  ASSERT_DOUBLE_EQ(0.75, env.GetValue());
  ASSERT_DOUBLE_EQ(1.0, env.GetValue());
  // Decay
  ASSERT_DOUBLE_EQ(0.8625, env.GetValue());
  ASSERT_DOUBLE_EQ(0.725, env.GetValue());
  ASSERT_DOUBLE_EQ(0.5875, env.GetValue());
  // Sustain
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.45, env.GetValue());
  }
  env.NoteOff();
  ASSERT_DOUBLE_EQ(0.39375, env.GetValue());
  ASSERT_DOUBLE_EQ(0.3375, env.GetValue());
  ASSERT_DOUBLE_EQ(0.28125, env.GetValue());
  ASSERT_DOUBLE_EQ(0.225, env.GetValue());
  ASSERT_DOUBLE_EQ(0.16875, env.GetValue());
  ASSERT_DOUBLE_EQ(0.1125, env.GetValue());
  ASSERT_DOUBLE_EQ(0.05625, env.GetValue());
  // Done
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.0, env.GetValue());
  }
}

static void TestAttackRelease() {
  envelope::Envelope env;
  env.set_attack(4);
  env.set_decay(0);
  env.set_sustain(0.0);
  env.set_release(8);
  env.NoteOn();
  // Attack
  ASSERT_DOUBLE_EQ(0.25, env.GetValue());
  ASSERT_DOUBLE_EQ(0.5, env.GetValue());
  ASSERT_DOUBLE_EQ(0.75, env.GetValue());
  env.NoteOff();
  // Released before we finished attacking.  No sustain so its always 0.
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.0, env.GetValue());
  }
}

static void TestDecay() {
  envelope::Envelope env;
  env.set_attack(0);
  env.set_decay(5);
  env.set_sustain(0.0);
  env.set_release(8);
  env.NoteOn();
  ASSERT_DOUBLE_EQ(0.8, env.GetValue());
  ASSERT_DOUBLE_EQ(0.6, env.GetValue());
  ASSERT_DOUBLE_EQ(0.4, env.GetValue());
  ASSERT_DOUBLE_EQ(0.2, env.GetValue());
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.0, env.GetValue());
  }
  env.NoteOff();
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.0, env.GetValue());
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  TestFlat();
  TestZero();
  TestCurve();
  TestAttackRelease();
  TestDecay();
  std::cout << "PASS" << std::endl;
  return 0;
}
