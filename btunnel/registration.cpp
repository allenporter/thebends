// registration.cpp
// Author: Allen Porter <allen@thebends.org>

#include "registration.h"

#include <string>
#include <err.h>
#include <dns_sd.h>

using namespace std;

namespace btunnel {

class RegisteredServiceImpl : public RegisteredService {
 public:
  RegisteredServiceImpl() : service_(NULL), name_(""), type_(""), domain_(""),
                            port_(0) { }

  virtual ~RegisteredServiceImpl() {
    if (service_ != NULL) {
      DNSServiceRefDeallocate(service_);
    }
  }

  void set_service(DNSServiceRef service) { service_ = service; }

  virtual const string& name() const { return name_; }
  void set_name(const string& name) { name_ = name; }

  virtual const string& type() const { return type_; }
  void set_type(const string& type) { type_ = type; }

  virtual const string& domain() const { return domain_; }
  void set_domain(const string& domain) { domain_ = domain; }

  virtual uint16_t port() const { return port_; }
  void set_port(uint16_t port) { port_ = port; }

 private:
  DNSServiceRef service_;  
  string name_;
  string type_;
  string domain_;
  uint16_t port_;
};

static void RegistrationCallback(DNSServiceRef service, DNSServiceFlags flags,
                                 DNSServiceErrorType result, const char *name,
                                 const char *regtype, const char *domain,
                                 void *context) {
  if (result != kDNSServiceErr_NoError) {
    // Handle with DNSServiceProcessResult() in NewService().
    return;
  }
  RegisteredServiceImpl* registered_service = (RegisteredServiceImpl*)context;
  registered_service->set_service(service);
  registered_service->set_type(regtype);
  registered_service->set_name(name);
  registered_service->set_domain(domain);
}

RegisteredService* NewRegisteredService(const string& type, uint16_t port) {
  RegisteredServiceImpl* registered_service = new RegisteredServiceImpl;
  registered_service->set_port(port);

  DNSServiceRef service;
  DNSServiceErrorType result;
  result = DNSServiceRegister(&service, 1, 0, NULL,
                              type.c_str(), NULL, NULL, htons(port), 0, NULL,
                              &RegistrationCallback, registered_service);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceRegister failed: %d", result);
    delete registered_service;
    return NULL;
  }
  result = DNSServiceProcessResult(service);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceProcessResult failed: %d", result);
    delete registered_service;
    return NULL;
  }
  return registered_service;
}

}  // namespace btunnel
