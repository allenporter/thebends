// registration.h
// Autor: Allen Porter <allen@thebends.org>
//
// Manages registration of bonjour services, a thin wrapper around the
// APIs exposed by dns_sd.h.

#include <string>
#include <sys/types.h>

namespace btunnel {

class RegisteredService {
 public:
  virtual ~RegisteredService() { }

  // TODO: Document accessors
  virtual const std::string& name() const = 0;
  virtual const std::string& type() const = 0;
  virtual const std::string& domain() const = 0;
  virtual uint16_t port() const = 0;

 protected:
  // Cannot be instantiated directly
  RegisteredService() { }
};

// TODO: Support custom service names
RegisteredService* NewRegisteredService(const std::string& type, uint16_t port);

}  // namespace btunnel
