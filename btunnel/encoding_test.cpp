// encoding_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include <string>
#include <time.h>
#include <stdlib.h>
#include "encoding.h"
#include "buffer.h"
#include "test_util.h"

using namespace std;

static void test1() {
  btunnel::Buffer buffer;
  for (int i = 0; i < 10; i++) {
    int length = random() % 1024;
    int expected_size = length + 2;  // sizeof(int16_t)
    string str = RandString(length);
    assert(expected_size == btunnel::WriteString(&buffer, 1024, str));
    string result;
    assert(expected_size == btunnel::ReadString(&buffer, 1024, &result));
    assert(result == str);
  }
}

static void test2() {
  btunnel::Buffer buffer;
  string result;
  assert(0 == btunnel::ReadString(&buffer, 1024, &result));
  assert(buffer.Append("\x00\x07", 2));
  assert(0 == btunnel::ReadString(&buffer, 1024, &result));
  assert(buffer.Append("abcde", 5));
  assert(0 == btunnel::ReadString(&buffer, 1024, &result));
  assert(buffer.Append("fg", 2));
  assert(9 == btunnel::ReadString(&buffer, 1024, &result));
}

static void test3() {
  btunnel::Buffer buffer;
  string result;
  assert(0 == btunnel::ReadString(&buffer, 6, &result));
  assert(buffer.Append("\x00\x07", 7));
  assert(-1 == btunnel::ReadString(&buffer, 6, &result));
}

static void test4() {
  btunnel::Buffer buffer;
  for (int i = 0; i < 10; i++) {
    assert(3 == btunnel::WriteString(&buffer, 6, "a"));
    assert(-1 == btunnel::WriteString(&buffer, 6, "zzzzzzzzzzzz"));
  }
}

int main(int argc, char* argv[]) {
  srandom(time(NULL));
  test1();
  test2();
  test3();
  test4();
  cout << "PASS" << endl;
}
