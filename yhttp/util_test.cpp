#include <iostream>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include "util.h"

using namespace std;
using namespace yhttpserver;

static void test1() {
  vector<string> parts;
  Split("", ' ', &parts);
  assert(parts.size() == 0);
}

static void test2() {
  vector<string> parts;
  Split("a", ' ', &parts);
  assert(parts.size() == 1);
  assert(parts[0] == "a");
}

static void test3() {
  vector<string> parts;
  Split("a ", ' ', &parts);
  assert(parts.size() == 1);
  assert(parts[0] == "a");
}

static void test4() {
  vector<string> parts;
  Split("a b", ' ', &parts);
  assert(parts.size() == 2);
  assert(parts[0] == "a");
  assert(parts[1] == "b");
}

static void test5() {
  vector<string> parts;
  Split("a   b", ' ', &parts);
  assert(parts.size() == 2);
  assert(parts[0] == "a");
  assert(parts[1] == "b");
}

static void test6() {
  vector<string> parts;
  Split("ab cd e  ", ' ', &parts);
  assert(parts.size() == 3);
  assert(parts[0] == "ab");
  assert(parts[1] == "cd");
  assert(parts[2] == "e");
}

static void test7() {
  vector<std::string> parts;
  yhttpserver::Split("a b c", ' ', &parts);
  assert(parts.size() == 3);
  assert(parts[0] == "a");
  assert(parts[1] == "b");
  assert(parts[2] == "c");
}

static void test8() {
  assert(!IsAlpha(""));
  assert(IsAlpha("a"));
  assert(!IsAlpha("1"));
  assert(IsAlpha("abcdef"));
  assert(!IsAlpha("abcdef "));
}

int main(int argc, char* argv[]) {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
  test7();
  test8();
  cout << "PASS" << endl;
}
