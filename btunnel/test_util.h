// test_util.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __BTUNNEL_TEST_UTIL_H__
#define __BTUNNEL_TEST_UTIL_H__

#include <string>

namespace test {

std::string RandString(int length);

int RandomPort();

}  // namespace test

#endif  // __BTUNNEL_TEST_UTIL_H__
