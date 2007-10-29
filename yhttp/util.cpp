#include "util.h"
#include <ctype.h>

namespace yhttpserver {

void Split(const std::string& message, char c,
           std::vector<std::string>* parts) {
  parts->clear();
  const char *last = message.c_str();
  const char *end = last + message.size();
  const char *cur = message.c_str();
  while (cur < end) {
    if (*cur == c) {
      if ((cur - last) != 0) {
        parts->push_back(std::string(last, cur - last));
      }
      last = cur + 1;
    }
    cur++;
  }
  if (last != end) {
    parts->push_back(std::string(last, cur - last));
  }
}

bool IsAlpha(const std::string& str) {
  for (size_t i = 0; i < str.size(); ++i) {
    if (!isalpha(str[i])) {
      return false;
    }
  }
  return str.size() > 0;
}

}  // namespace yhttpserver
