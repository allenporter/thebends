#include <iostream>
#include <string>
#include <assert.h>
#include <stdlib.h>
#include "url.h"

using namespace std;
using namespace yhttpserver;

static void test1() {
  URL url("");
  assert(!url.IsValid());
}

static void test2() {
  URL url("http://www.foo.com");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.Host() == "www.foo.com");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Port() == 80);
  assert(url.Path() == "");
  assert(url.Query() == "");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 0);
}

static void test3() {
  URL url("http://www.foo.com/");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "");
  assert(url.Query() == "");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 0);
}

static void test4() {
  URL url("http://www.foo.com:8080/bar");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 8080);
  assert(url.Path() == "bar");
  assert(url.Query() == "");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 0);
}

static void test5() {
  URL url("http://www.foo.com/bar?");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "bar");
  assert(url.Query() == "");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 0);
}

static void test6() {
  URL url("http://www.foo.com/bar?q");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "bar");
  assert(url.Query() == "q");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 1);
  assert((*params)["q"] == "");
}

static void test7() {
  URL url("http://www.foo.com/bar?q=foo");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "bar");
  assert(url.Query() == "q=foo");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 1);
  assert((*params)["q"] == "foo");
}

static void test8() {
  URL url("http://www.foo.com/bar?q=foo&bar=?");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "bar");
  assert(url.Query() == "q=foo&bar=?");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 2);
  assert((*params)["q"] == "foo");
  assert((*params)["bar"] == "?");
}

static void test9() {
  URL url("http://user@www.foo.com");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "user");
  assert(url.Password() == "");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "");
  assert(url.Query() == "");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 0);
}

static void test10() {
  URL url("http://user:pass@www.foo.com///bar?q=a%3cb%3E&u=1%3");
  assert(url.IsValid());
  assert(url.Scheme() == "http");
  assert(url.User() == "user");
  assert(url.Password() == "pass");
  assert(url.Host() == "www.foo.com");
  assert(url.Port() == 80);
  assert(url.Path() == "bar");
  assert(url.Query() == "q=a%3cb%3E&u=1%3");
  URL::ParamMap* params = url.Params();
  assert(params->size() == 2);
  assert((*params)["q"] == "a<b>");
  assert((*params)["u"] == "1%3");
}

static void test11() {
  URL url("http://user:pass@www.foo.com@bar");
  assert(!url.IsValid());
}

static void test12() {
  URL url("http://user:pass:www.foo.com/bar");
  assert(!url.IsValid());
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
  test9();
  test10();
  test11();
  test12();
  cout << "PASS" << endl;
}
