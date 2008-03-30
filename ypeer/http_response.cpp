// response.cpp
// Author: Allen Porter <allen@thebends.org>

#include "http_response.h"
#include <iostream>
#include <string>

using namespace std;

namespace ypeer {

bool Has200(const string& header) {
  if (header.find("HTTP/") != 0) {
    cerr << "Invalid HTTP response header" << endl;
    return false;
  }
  size_t code_start = header.find(" ", 5) + 1;
  if (code_start == string::npos) {
    cerr << "Invalid HTTP start of response code" << endl;
    return false;
  } 
  size_t code_end = header.find(" ", code_start);
  if (code_end == string::npos) {
    cerr << "Invalid HTTP end of response code" << endl;
    return false;
  } 
  size_t length = code_end - code_start;
  string c = header.substr(code_start, 3);
  if (length != 3) {
    cerr << "Invalid HTTP response code length (" << length << ")" << endl;
    return false;
  }
  string code = header.substr(code_start, 3);
  assert(code.size() == 3);
  if (code != "200") {
    cerr << "Invalid HTTP response code: " << code << endl;
    return false;
  }
  return true;
}

bool ParseHTTPResponse(const string& input, string* body) {
  size_t body_start = input.find("\r\n\r\n");
  if (body_start == string::npos) {
    cerr << "Invalid HTTP body start" << endl;
    return false;
  }
  const string& header = input.substr(0, body_start);
  if (!Has200(header)) {
    return false;
  }
  *body = input.substr(body_start + 4);
  return true;
}

}  // namespace ypeer
