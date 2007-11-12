// registration.h
// Autor: Allen Porter <allen@thebends.org>
//
// Manages registration of bonjour services, a thin wrapper around the
// APIs exposed by dns_sd.h.

#include <map>
#include <sys/types.h>
#include <dns_sd.h>

namespace btunnel {

class Service;

class ServiceManager {
 public:
  ServiceManager();
  ~ServiceManager();

  bool Register(Service* service);
  bool Unregister(Service* service);

 private:
  typedef std::map<Service*, DNSServiceRef> ServiceMap;
  ServiceMap services_;
};

}  // namespace btunnel
