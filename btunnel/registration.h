// registration.h
// Autor: Allen Porter <allen@thebends.org>
//
// Manages registration of bonjour services, a thin wrapper around the
// APIs exposed by dns_sd.h.

#include <map>
#include <string>
#include <sys/types.h>

namespace btunnel {

class Service;

// TODO: Support custom service names
Service* NewRegisteredService(
    const std::string& type, uint16_t port,
    const std::map<std::string, std::string>& txt_records);

}  // namespace btunnel
