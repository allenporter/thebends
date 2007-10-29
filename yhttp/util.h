// util.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __YHTTPSERVER_UTIL_H__
#define __YHTTPSERVER_UTIL_H__

#include <string>
#include <vector>

namespace yhttpserver {

void Split(const std::string& message, char c, std::vector<std::string>* parts);

bool IsAlpha(const std::string& str);

}  // namespace yhttpserver

#endif  // __YHTTPSERVER_UTIL_H__
