// messages.h
// Author: Allen Porter <allen@thebends.org>

#include "messages.h"
#include <assert.h>
#include <err.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <libnet.h>
#include "options.h"

#define MTU 1500
#define MAX_OPTION_SIZE 512  // arbitrarily chosen
#define LEASE_TIME 7776000

// Options we'll request from the server
static uint8_t request_params[] = {
    DHCP_SUBNET_MASK, DHCP_ROUTER, DHCP_DNS, DHCP_DOMAIN
  };

static void build_msg(uint8_t op, uint8_t* source_mac, uint32_t xid,
                      struct dhcp_message* m) {
  memset(m, 0, sizeof(struct dhcp_message));
  m->op = op;
  m->htype = 1;
  m->hlen = 6;
  m->hops = 0;
  m->xid = xid;
  m->secs = 0;
  m->flags = 0;
  memcpy(m->chaddr, source_mac, 6);
  m->options_signature[0] = 99;
  m->options_signature[1] = 130;
  m->options_signature[2] = 83;
  m->options_signature[3] = 99;
}

int build_dhcp_discover(uint8_t* source_mac, const char* hostname,
                        uint8_t* buf, uint16_t* size) {
  if (*size < (sizeof(struct dhcp_message) + MAX_OPTION_SIZE)) {
    warnx("build_discover(): Provided buffer was too small");
    return -1;
  }
  struct dhcp_message* m = (struct dhcp_message*)buf;
  build_msg(BOOTREQUEST, source_mac, random(), m);

  // Build DHCPDISCOVER options
  struct options opts;
  memset(&opts, 0, sizeof(struct options));
  opts.message_type = MESSAGE_DHCPDISCOVER;
  opts.max_size = MTU;
  opts.lease_time = LEASE_TIME;
  // Client ID
  opts.client_id_type = 1;
  opts.client_id_length = 6;
  memcpy(opts.client_id, source_mac, 6);
  // Requested parameters
  opts.param_size = sizeof(request_params);
  memcpy(opts.param_list, request_params, sizeof(request_params));
  // Hostname
  strncpy(opts.hostname, hostname, OPTION_FIELD_MAX - 1);
  opts.hostname[OPTION_FIELD_MAX - 1] = '\0';

  uint16_t options_size = MAX_OPTION_SIZE;
  write_options(&opts, buf + sizeof(struct dhcp_message), &options_size);

  *size = sizeof(struct dhcp_message) + options_size;
  return 0;
}

int build_dhcp_request(uint32_t xid, uint8_t* source_mac, const char* hostname,
                       uint32_t requested_ip, uint32_t server_id,
                       uint8_t* buf, uint16_t* size) {
  if (*size < (sizeof(struct dhcp_message) + MAX_OPTION_SIZE)) {
    warnx("build_discover(): Provided buffer was too small");
    return -1;
  }
  struct dhcp_message* m = (struct dhcp_message*)buf;
  build_msg(BOOTREQUEST, source_mac, xid, m);

  // Build DHCPDISCOVER options
  struct options opts;
  memset(&opts, 0, sizeof(struct options));
  opts.message_type = MESSAGE_DHCPREQUEST;
  opts.max_size = MTU;
  // Client ID
  opts.client_id_type = 1;
  opts.client_id_length = 6;
  memcpy(opts.client_id, source_mac, 6);
  // Requested parameters
  opts.param_size = sizeof(request_params);
  memcpy(opts.param_list, request_params, sizeof(request_params));
  // Hostname
  strncpy(opts.hostname, hostname, OPTION_FIELD_MAX - 1);
  opts.hostname[OPTION_FIELD_MAX - 1] = '\0';

  opts.requested_ip = requested_ip;
  opts.server_id = server_id;

  uint16_t options_size = MAX_OPTION_SIZE;
  write_options(&opts, buf + sizeof(struct dhcp_message), &options_size);

  *size = sizeof(struct dhcp_message) + options_size;
  return 0;
}

