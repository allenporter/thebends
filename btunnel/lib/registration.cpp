// registration.cpp
// Author: Allen Porter <allen@thebends.org>

#include "registration.h"

#include <string>
#include <err.h>
#include <dns_sd.h>
#include <iostream>
#include "service.h"

using namespace std;

namespace btunnel {

ServiceManager::ServiceManager() { }

ServiceManager::~ServiceManager() {
  // Must unregister all services first
  assert(services_.size() == 0);
}

static void RegistrationCallback(DNSServiceRef service, DNSServiceFlags flags,
                                 DNSServiceErrorType result, const char *name,
                                 const char *regtype, const char *domain,
                                 void *context) {
  if (result != kDNSServiceErr_NoError) {
    // Handle with DNSServiceProcessResult()
    return;
  }
}

bool ServiceManager::Register(Service* service) {
  // Host and domain are unused; Sanity check that the caller didn't specify
  assert(service->domain().size() == 0);
  assert(service->host().size() == 0);

  DNSServiceRef dns_service;
  DNSServiceErrorType result;
  result = DNSServiceRegister(&dns_service, 1, 0,
                              service->name().c_str(),
                              service->type().c_str(),
                              NULL, // TODO: domain needed?
                              NULL, // TODO: host needed?
                              htons(service->port()),
                              service->txt().size(), service->txt().data(),
                              RegistrationCallback, NULL);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceRegister failed: %d", result);
    return false;
  }
  result = DNSServiceProcessResult(dns_service);
  if (result != kDNSServiceErr_NoError) {
    warn("DNSServiceProcessResult failed: %d", result);
    return false;
  }
  services_[service] = dns_service;
  return true;
}

bool ServiceManager::Unregister(Service* service) {
  ServiceMap::iterator iter = services_.find(service);
  if (iter == services_.end()) {
    return false;
  }
  DNSServiceRefDeallocate(iter->second);
  services_.erase(iter);
  return true;
}

}  // namespace btunnel
