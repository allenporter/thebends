// options.h
// Author: Allen Porter <allen@thebends.org>
//
// Implements a subset of options provided by RFC 2132.  This class only
// implements the options used in other parts of this library, and ignores
// all other options.

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

#include <sys/types.h>

// Option codes
#define DHCP_PAD 0
#define DHCP_SUBNET_MASK 1
#define DHCP_ROUTER 3
#define DHCP_DNS 6
#define DHCP_DOMAIN 15
#define DHCP_HOST_NAME 12
#define DHCP_REQUESTED_IP 50
#define DHCP_LEASE_TIME 51
#define DHCP_OVERLOAD 52
#define DHCP_MESSAGE_TYPE 53
#define DHCP_SERVER_ID 54
#define DHCP_PARAMETER_LIST 55
#define DHCP_MAX_SIZE 57
#define DHCP_CLIENT_ID 61
#define DHCP_END 255

// Values for the message_type options field
#define MESSAGE_DHCPDISCOVER 1
#define MESSAGE_DHCPOFFER 2
#define MESSAGE_DHCPREQUEST 3
#define MESSAGE_DHCPDECLINE 4
#define MESSAGE_DHCPACK 5
#define MESSAGE_DHCPNAK 6
#define MESSAGE_DHCPRELEASE 7
#define MESSAGE_DHCPINFORM 8

#define OPTION_FIELD_MAX 64  // arbitrarily chosen

// The implemented options are chosen based on which options I found when
// sniffing my home network.
struct options {
  // DHCP message type identifying the request
  uint8_t message_type;

  // Maximum DHCP message size willing to accept, only for DHCPDISCOVER and
  // DHCPREQUEST, not in DHCPDECLINE.  Minimum legal value is 576.
  uint16_t max_size;

  // Name of the client, NULL terminated string.
  char hostname[OPTION_FIELD_MAX];

  // Requested IP address, used during DHCPDISCOVER
  uint32_t requested_ip;

  // Used in DHCPDISCOVER or DHCPREQUEST to specify a lease time for the IP
  // in seconds.
  uint32_t lease_time;

  // Server-identifier (ip address), included in DHCPOFFER and DHCPREQUEST
  // messages
  uint32_t server_id;

  // Subnet mask, per RPC 950
  uint32_t subnet;

  // Defines a list of routers (max of OPTION_FIELD_MAX / 4)
  uint8_t router_num;
  uint32_t router[OPTION_FIELD_MAX];

  // Defines a list of DNS Servers (max of OPTION_FIELD_MAX / 4)
  uint8_t dns_num;
  uint32_t dns[OPTION_FIELD_MAX];

  // Domain name that client should use when resolving hostnames via DNS
  char domain[OPTION_FIELD_MAX];

  // Client-identifier
  // client_id_type should be 0 when client_id is not a hardware type,
  // and non-zero when a hardware address is specified, with the value of the
  // arp hardware type.
  uint8_t client_id_type;
  uint8_t client_id_length;
  uint8_t client_id[OPTION_FIELD_MAX];

  // Requested parameter list (DHCP option codes)
  uint8_t param_size;
  uint8_t param_list[OPTION_FIELD_MAX];
};

// Parses a buffer containing the DHCP options from a message, populating
// the specified options struct.  Returns -1 if a parse failure occurs.
int parse_options(const uint8_t* buf, int buf_len, struct options* opts);

// Outputs the specified DHCP options to a buffer
void write_options(struct options* opts, uint8_t* buf, uint16_t* size);

#endif  // __OPTIONS_H__
