// service.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __BTUNNEL_SERVICE_H__
#define __BTUNNEL_SERVICE_H__

#include <map>
#include <string>

namespace btunnel {

class Service {
 public:
  Service(const std::string& name,
          const std::string& type,
          const std::string& domain,
          const std::string& host,
          uint16_t port,
          const std::string& txt);

  Service(const std::string& name,
          const std::string& type,
          const std::string& domain,
          const std::string& host,
          uint16_t port,
          const std::map<std::string, std::string>& txt_records);

  virtual ~Service();

  // TODO: Document accessors
  const std::string& name() const;
  const std::string& type() const;
  const std::string& domain() const;

  const std::string& host() const;
  uint16_t port() const;

  const std::string& txt() const;
  const std::map<std::string, std::string>& txt_records() const;

 protected:
  std::string name_;
  std::string type_;
  std::string domain_;
  std::string host_;
  uint16_t port_;
  std::string txt_;
  std::map<std::string, std::string> txt_records_;
};

}  // namespace btunnel

#endif  // __BTUNNEL_SERVICE_H__
