// browse.cpp
// Author: Allen Porter <allen@thebends.org>

#include <err.h>
#include <dns_sd.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "browse.h"
#include "service.h"

using namespace std;

namespace btunnel {

struct ResolveContext {
  string host;
  uint16_t port;
  string txt;
};

static void ResolveCallback(DNSServiceRef client,
                            DNSServiceFlags flags,
                            uint32_t interfaceIndex,
                            DNSServiceErrorType result,
                            const char *name,
                            const char *host_target,
                            uint16_t port,
                            uint16_t txt_len,
                            const char *txt,
                            void *context) {
  cout << "DEBUG: ResolveCallback, flags=" << flags << endl;
  if (result != kDNSServiceErr_NoError) {
    warn("ResolveCallback invoked with: %d", result);
    return;
  }
  struct ResolveContext* ctx = (struct ResolveContext*)context;
  ctx->host = string(host_target);
  ctx->port = port;
  ctx->txt = string(txt, txt_len);
}

struct BrowseContext {
  bool more;
  vector<Service*>* services;
};

static void BrowseCallback(DNSServiceRef service,
                           DNSServiceFlags flags,
                           uint32_t interfaceIndex,
                           DNSServiceErrorType result,
                           const char *name,
                           const char *regtype,
                           const char *domain,
                           void *context) {
  if (result != kDNSServiceErr_NoError) {
    warn("BrowseCallback invoked with: %d", result);
    return;
  }
  // TODO: Support "remove" flags here
  cout << "DEBUG: BrowseCallback, flags=" << flags << endl;
  struct BrowseContext* ctx = (struct BrowseContext*)context;
  ctx->more = (flags & kDNSServiceFlagsMoreComing);

  struct ResolveContext resolve_ctx;
  DNSServiceRef resolve_client;
  result = DNSServiceResolve(&resolve_client, 0, 0, name, regtype, domain,
                             ResolveCallback, &resolve_ctx);
  if (result != kDNSServiceErr_NoError) {
    DNSServiceRefDeallocate(resolve_client);
    errx(1, "DNSServiceResolve failed: %d", result);
  }

  result = DNSServiceProcessResult(resolve_client);
  if (result != kDNSServiceErr_NoError) {
    DNSServiceRefDeallocate(resolve_client);
    errx(1, "DNSServiceProcessResult (resolve) failed: %d", result);
  }
  ctx->services->push_back(new Service(name, regtype, domain, resolve_ctx.host,
                                       resolve_ctx.port,
                                       resolve_ctx.txt));
}

void BrowseService(const string& service_type,
                   vector<Service*>* services) {
  services->clear();
  DNSServiceErrorType result;
  DNSServiceRef service;

  struct BrowseContext context;
  context.more = true;
  context.services = services;

  result = DNSServiceBrowse(&service, 0, 0, service_type.c_str(),
                            NULL, BrowseCallback, &context);
  if (result != kDNSServiceErr_NoError) {
    DNSServiceRefDeallocate(service);
    errx(1, "DNSServiceRegister failed: %d", result);
  }
  // The callback checks if more results are available, and sets more to false
  // when done.
  while (context.more) {
    result = DNSServiceProcessResult(service);
    if (result != kDNSServiceErr_NoError) {
      DNSServiceRefDeallocate(service);
      errx(1, "DNSServiceProcessResult (browse) failed: %d", result);
    }
  }
}

}  // namespace btunnel
