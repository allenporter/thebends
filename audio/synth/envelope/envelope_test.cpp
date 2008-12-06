// envelope_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <assert.h>
#include <iostream>
#include "envelope/envelope.h"

#define ASSERT_DOUBLE_EQ(x, y) (assert((x - y) < 0.0000001))

namespace {

static void TestFlat() {
  envelope::Envelope env;
  env.set_attack(1.0);
  env.set_decay(0.0);
  env.set_sustain(1.0);
  env.set_release(1.0);
  env.NoteOn();
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(1.0, env.GetValue());
  }
  env.NoteOff();
  assert(0.0 == env.GetValue());
}

static void TestZero() {
  envelope::Envelope env;
  env.set_attack(0.0);
  env.set_decay(0.0);
  env.set_sustain(0.0);
  env.set_release(0.0);
  env.NoteOn();
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.0, env.GetValue());
  }
  env.NoteOff();
  ASSERT_DOUBLE_EQ(0.0, env.GetValue());
}

static void TestCurve() {
  envelope::Envelope env;
  env.set_attack(0.25);
  env.set_decay(0.20);
  env.set_sustain(0.45);
  env.set_release(0.10);
  env.NoteOn();
  ASSERT_DOUBLE_EQ(0.25, env.GetValue());
  ASSERT_DOUBLE_EQ(0.5, env.GetValue());
  ASSERT_DOUBLE_EQ(0.75, env.GetValue());
  ASSERT_DOUBLE_EQ(1.0, env.GetValue());
  ASSERT_DOUBLE_EQ(0.80, env.GetValue());
  ASSERT_DOUBLE_EQ(0.60, env.GetValue());
  for (int i = 0; i < 10; ++i) {
    ASSERT_DOUBLE_EQ(0.45, env.GetValue());
  }
  env.NoteOff();
  ASSERT_DOUBLE_EQ(0.35, env.GetValue());
  ASSERT_DOUBLE_EQ(0.25, env.GetValue());
  ASSERT_DOUBLE_EQ(0.15, env.GetValue());
  ASSERT_DOUBLE_EQ(0.05, env.GetValue());
  ASSERT_DOUBLE_EQ(0.0, env.GetValue());
}

}  // namespace

int main(int argc, char* argv[]) {
  TestFlat();
  TestZero();
  TestCurve();
  std::cout << "PASS" << std::endl;
  return 0;
}
