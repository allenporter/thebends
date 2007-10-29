// util.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __UTIL_H__
#define __UTIL_H__

#include <inttypes.h>

void ntohs(uint16_t* value);

void ntohl(uint32_t* value);

int inet_aton(const char* cp, uint32_t* ip);

#endif  // __UTIL_H__
