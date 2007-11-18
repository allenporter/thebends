// test_util.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __BTUNNEL_TEST_UTIL_H__
#define __BTUNNEL_TEST_UTIL_H__

#include <string>

static std::string RandString(int length) {
  std::string s;
  for (int i = 0; i < length; ++i) {
    char c = random() % 255;
    s.append(&c, 1);
  }
  return s;
}

#endif  // __BTUNNEL_TEST_UTIL_H__
