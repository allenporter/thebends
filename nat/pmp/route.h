// route.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __PMP_ROUTE_H__
#define __PMP_ROUTE_H__

#include <arpa/inet.h>

namespace pmp {

// Returns the default route
bool GetDefaultGateway(in_addr* addr);

}  // pmp

#endif  // __PMP_ROUTE_H__
