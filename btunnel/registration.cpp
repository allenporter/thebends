// registration.cpp
// Author: Allen Porter <allen@thebends.org>

#include "registration.h"

#include <string>
#include <err.h>
#include <dns_sd.h>
#include "service.h"

using namespace std;

namespace btunnel {

class RegisteredService : public Service {
 public:
  RegisteredService(DNSServiceRef service, 
                    const string& name,
                    const string& type,
                    const string& domain,
                    const string& host,
                    uint16_t port,
                    const map<string, string>& txt_records)
      : Service(name, type, domain, host, port, txt_records),
        service_(service) {
  }

  virtual ~RegisteredService() {
    DNSServiceRefDeallocate(service_);
  }

 private:
  DNSServiceRef service_;  
};

struct Context {
  RegisteredService* service;
  uint16_t port;
  map<string, string> txt_records;
};

static void RegistrationCallback(DNSServiceRef service, DNSServiceFlags flags,
                                 DNSServiceErrorType result, const char *name,
                                 const char *regtype, const char *domain,
                                 void *context) {
  if (result != kDNSServiceErr_NoError) {
    // Handle with DNSServiceProcessResult() in NewService().
    return;
  }
  struct Context* ctx = (struct Context*)context;
  ctx->service = new RegisteredService(service, name, regtype, domain,
                                       "localhost", ctx->port,
                                       ctx->txt_records);
}

Service* NewRegisteredService(const string& type, uint16_t port,
                              const map<string, string>& txt_records) {
  struct Context context;
  context.port = port;
  context.txt_records = txt_records;

  DNSServiceRef service;
  DNSServiceErrorType result;
  result = DNSServiceRegister(&service, 1, 0, NULL,
                              type.c_str(), NULL, NULL, htons(port), 0, NULL,
                              &RegistrationCallback, &context);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceRegister failed: %d", result);
    return NULL;
  }
  result = DNSServiceProcessResult(service);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceProcessResult failed: %d", result);
    return NULL;
  }
  result = DNSServiceUpdateRecord(service, NULL, 0,
                                  context.service->txt().size(),
                                  context.service->txt().data(), 900);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceUpdateRecord failed: %d", result);
    return NULL;
  }
  return context.service;
}

}  // namespace btunnel
