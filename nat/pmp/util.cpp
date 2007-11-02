// util.cpp
// Author: Allen Porter <allen@thebends.org>
#include "util.h"

#include <assert.h>
#include <err.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <sys/types.h>
#include "message.h"
#include "op.h"
#include "route.h"

namespace pmp {

bool GetGatewayInfo(in_addr* public_ip, uint32_t* uptime) {
  struct sockaddr_in name;
  if (!GetPortMapAddress(&name)) {
    return false;
  }

  Op* op = NewUdpOp();

  struct public_ip_request request;
  bzero(&request, sizeof(struct public_ip_request));
  struct public_ip_response resp;
  if (!op->Run(&name, (const char*)&request,
               sizeof(struct public_ip_request),
               (char*)&resp, sizeof(struct public_ip_response))) {
    return false;
  }
  public_ip->s_addr = resp.ip.s_addr;
  *uptime = ntohl(resp.epoch);
  return true;
}

}  // namespace pmp

