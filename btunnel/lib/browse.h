// registration.h
// Autor: Allen Porter <allen@thebends.org>
//
// Manages registration of bonjour services, a thin wrapper around the
// APIs exposed by dns_sd.h.

#ifndef __BTUNNEL_BROWSE_H__
#define __BTUNNEL_BROWSE_H__

#include <string>
#include <vector>
#include <sys/types.h>

namespace btunnel {

class Service;

// Returns a vector of registered services of the specified type, such as
// "_daap._tcp".  The caller is responsible for freeing all of the returned
// Service objects.
void BrowseService(const std::string& service_type,
                   std::vector<Service*>* services);

}  // namespace btunnel

#endif  // __BTUNNEL_BROWSE_H__
