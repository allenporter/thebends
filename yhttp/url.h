// url.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __YHTTPSERVER_URL_H__
#define __YHTTPSERVER_URL_H__

#include <map>
#include <string>

namespace yhttpserver {

class URL {
 public:
  typedef std::map<std::string, std::string> ParamMap;

  URL();
  explicit URL(const std::string& url);
  ~URL();

  void Parse(const std::string& url);

  bool IsValid() const { return valid_; }
  const std::string User() const { return user_; }
  const std::string Password() const { return password_; }
  const std::string Scheme() const { return scheme_; }
  const std::string Host() const { return host_; }
  uint32_t Port() const { return port_; }
  const std::string Path() const { return path_; }
  const std::string Query() const { return query_; }
  ParamMap* Params() { return &params_; }

 private:
  void Parse();

  bool valid_;
  std::string url_;
  std::string scheme_;
  std::string user_;
  std::string password_;
  std::string host_;
  uint32_t port_;
  std::string path_;
  std::string query_;
  ParamMap params_;
};

std::string Unescape(const std::string& str);

}  // namespace yhttpserver

#endif  // __YHTTPSERVER_URL_H__
