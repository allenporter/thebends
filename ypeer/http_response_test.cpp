// http_response_test.cpp
// Author: Allen Porter <allen@thebends.org>

#include <iostream>
#include <string>
#include "http_response.h"
#include <assert.h>

using namespace std;

static void test1() {
  string s = \
    "HTTP/1.0 200 OK\n" \
    "Content-Type: text/plain\n" \
    "Content-Length: 69\n" \
    "\n" \
    "d8:completei0e10:incompletei2e8:intervali1781e5:peers12:?ja*??ja*?e\n";
  string contents;
  assert(ypeer::ParseHTTPResponse(s, &contents));
  assert(contents ==
    "d8:completei0e10:incompletei2e8:intervali1781e5:peers12:?ja*??ja*?e");
}

static void test2() {
  string s = \
    "HTTP/1.0 404 Not Found\n" \
    "Cache-Control: private\n" \
    "Content-Type: text/html; charset=ISO-8859-1\n" \
    "Server: gws\n" \
    "Date: Sat, 29 Mar 2008 23:36:06 GMT\n" \
    "Connection: Close\n"
    "\n"
    "<html></html>\n";
  string contents;
  assert(!ypeer::ParseHTTPResponse(s, &contents));
  assert(contents.size() == 0);
}

int main(int argc, char* argv) {
  test1();
  test2();
}
