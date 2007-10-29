// Somewhat based on RFC 1738, somewhat ignored.
#include "url.h"
#include <stdlib.h>
#include <limits.h>
#include "util.h"

namespace yhttpserver {

static bool islowalpha(char c) {
  return (isalpha(c) && islower(c));
}

static bool issafe(char c) {
  return c == '$' || c == '-' || c == '_' || c == '.' || c == '+';
}

static bool isextra(char c) {
  return c == '!' || c == '*' || c == '\'' || c == '(' || c == ')' ||
         c == ',';
}

static bool isescape(char c) {
  return (c == '%');
}

static bool isunreserved(char c) {
  return isalpha(c) || isdigit(c) || issafe(c) || isextra(c);
}

static bool isreserved(char c) {
  return c == ';' || c == '/' || c == '?' || c == ':' || c == '@' ||
         c == '&' || c == '=';
}

static bool isscheme(char c) {
  return islowalpha(c) || isdigit(c) || c == '+' || c == '-' || c == '.';
}

static bool isxchar(char c) {
  return isunreserved(c) || isreserved(c) || isescape(c);
}

static bool isuchar(char c) {
  return isunreserved(c) || isescape(c);
}

static bool isuser(char c) {
  return isuchar(c) || c == ';' || c == '?' || c == '&' || c == '=';
}

static bool ishost(char c) {
  return isalpha(c) || isdigit(c) || c == '-' || c == '.';;
}

static char hextodec(char c) {
  assert(isxdigit(c));
  return isdigit(c) ? (c - '0') : (c - 'a' + 10);
}


URL::URL(const std::string&  url) {
  Parse(url);
}

URL::URL() : valid_(false) { }

URL::~URL() { }

static bool ConsumeScheme(std::string* url, std::string* scheme) {
  size_t size = url->size();
  const char* cur = url->c_str();
  const char* start = cur;
  while ((size_t)(cur - start) < size && isscheme(*cur)) {
    ++cur;
  }
  if (start == cur) {
    return false;
  }
  *scheme = std::string(start, cur - start);
  url->erase(0, scheme->size());
  if (url->find("://") != 0) {
    return false;
  }
  url->erase(0, 3);
  return true;
}

static bool ConsumeUserPass(std::string* url, std::string* token) {
  if (url->size() == 0) {
    return false;
  }
  size_t size = url->size();
  const char* cur = url->c_str();
  const char* start = cur;
  while (((size_t)(cur - start) < size - 1) &&
         (isuser(*cur) || ishost(*cur) || *cur == ':')) {
    ++cur;
  }
  if (start == cur) {
    return false;
  }
  if (*cur != '@') {
    return false;
  }
  token->clear();
  token->append(start, cur - start);
  url->erase(0, cur - start + 1);
  return true;
}

static bool ConsumeLogin(std::string* url, std::string* login,
                         std::string* password, std::string* host,
                         uint32_t* port) {
  if (url->size() == 0) {
    return false;
  }
  if (ConsumeUserPass(url, login)) {
    if (url->size() == 0)  {
      return false;
    }
    std::vector<std::string> login_parts;
    Split(*login, ':', &login_parts);
    if (login_parts.size() != 1 && login_parts.size() != 2) {
      return false;
    }
    *login = login_parts[0];
    if (login_parts.size() == 2) {
      *password = login_parts[1];
    }
  }
  if (url->size() == 0) {
    return false;
  }
  size_t end_pos = url->find("/");
  if (end_pos == std::string::npos) {
    end_pos = url->size();
  }
  host->clear();
  host->append(url->substr(0, end_pos));
  url->erase(0, end_pos);
  std::vector<std::string> parts;
  Split(*host, ':', &parts);
  if (parts.size() != 1 && parts.size() != 2) {
    return false;
  }
  if (parts.size() == 2) {
    *host = parts[0];
  }
  for (size_t i = 0; i < host->size(); ++i) {
    if (!ishost(host->at(i))) {
      return false;
    }
  }
  if (parts.size() == 2) {
    if (parts[1].size() == 0) {
      return false;
    }
    const std::string& port_str = parts[1];
    for (size_t i = 0; i < port_str.size(); ++i) {
      if (!isdigit(port_str[i])) {
        return false;
      }
    }
    *port = (uint32_t)strtoul(port_str.c_str(), NULL, 10);
  } else {
    *port = 80;
  }
  return true;
}

static bool ConsumePath(std::string* url, std::string* path,
                        std::string* query,
                        URL::ParamMap* params) {
  while (url->size() > 0 && url->at(0) == '/') {
    url->erase(0, 1);
  }
  for (size_t i = 0; i < url->size(); i++) {
    if (!isxchar(url->at(i))) {
      return false;
    }
  }
  size_t query_pos = url->find("?");
  if (query_pos != std::string::npos) {
    *path = url->substr(0, query_pos);
    *query = url->substr(query_pos + 1);
  } else {
    *path = *url;
    *query = "";
  }
  std::vector<std::string> pairs;
  Split(*query, '&', &pairs);
  for (std::vector<std::string>::const_iterator it = pairs.begin();
       it != pairs.end(); ++it) {
    std::vector<std::string> pair;
    Split(*it, '=', &pair);
    if ((pair.size() != 1 && pair.size() != 2) || pair[0].size() == 0) {
      return false;
    }
    if (pair.size() == 1) {
      (*params)[pair[0]] = "";
    } else {
      (*params)[pair[0]] = Unescape(pair[1]);
    }
  }
  url->clear();
  return true;
}

void URL::Parse(const std::string& url) {
  valid_ = false;
  scheme_.clear();
  user_.clear();
  password_.clear();
  host_.clear();
  port_ = 0;
  path_.clear();
  query_.clear();
  std::string tmp = url;
  if (!ConsumeScheme(&tmp, &scheme_) ||
      !ConsumeLogin(&tmp, &user_, &password_, &host_, &port_) ||
      !ConsumePath(&tmp, &path_, &query_, &params_)) {
    return;
  }
  valid_ = true;
}

std::string Unescape(const std::string& str) {
  std::string result;
  size_t cur = 0;
  while (cur < str.size()) {
    char c = str[cur];
    if (c == '%') {
      if (cur + 2 < str.size()) {
        char a = tolower(str[cur+1]);
        char b = tolower(str[cur+2]);
        if (isxdigit(a) && isxdigit(b)) {
          // Are we worried about NULLs here? I guess its not that big of a deal
          char value = (16 * hextodec(a) + hextodec(b));
          result.append(1, value);
          cur += 3;
          continue;
        }
      }
    }
    result.append(1, c);
    cur++;
  }
  return result;
}


}  // namespace yhttpserver
