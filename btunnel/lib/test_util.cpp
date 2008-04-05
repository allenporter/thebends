// test_util.cpp
// Author: Allen Porter <allen@thebends.org>

#include "test_util.h"
#include <string>
#include <stdlib.h>

using namespace std;

namespace test {

// static
string RandString(int length) {
  std::string s;
  for (int i = 0; i < length; ++i) {
    char c = random() % 255;
    s.append(&c, 1);
  }
  return s;
}

// static
int RandomPort() {
  return (random() % 1000) + 9000;
}

}  // namespace test
