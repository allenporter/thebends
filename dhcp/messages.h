// messages.h
// Author: Allen Porter <allen@thebends.org>
//
// RFC 2131

#ifndef __DHCP_H__
#define __DHCP_H__

#include <sys/types.h>

// Message types (not explicitly defined as a single field)
//
// DHCPDISCOVER
// Client broadcast to locate available servers
//
// DHCPOFFER
// Server to client in response to DHCPDISCOVER
//
// DHCPREQUEST
// Client message to servers either (a) requesting offered params from one
// server and implicitly declining offers from all others, (b) confirming
// correctness of previously allocated address (e.g., after reboot) (c)
// extending the lease on a particular network address.
//
// DHCPACK
// Server to client with configuration parameters including committed address
//
// DHCPNAK
// Server to client indicating client's notion is incorrect or lease expired
//
// DHCPDECLINE
// Client to server indicating address is already in use
//
// DHCPRELEASE
// Client to server relinquishing address, cancelling remaining lease
//
// DHCPINFORM
// Client to server, asking for local configuration parameters (client already
// has a network address)

#define MAX_DCHP_MESSAGE_SIZE 576

#define BOOTREQUEST 1
#define BOOTREPLY 2

struct dhcp_message {
  // Message op code / message type
  // 1 = BOOTREQUEST, 2 = BOOTREPLY
  uint8_t op;

  // Hardware address type (1 = 10mb ethernet)
  uint8_t htype;

  // Hardware address length (6 for 10mb ethernet)
  uint8_t hlen;

  // Client sets to 0, optionally used by relay agents
  uint8_t hops; 

  // Transation id, randomly chosen by the client and server
  uint32_t xid;

  // Filled in by client, seconds since address acquisition/renewal  started
  uint16_t secs;

  // [0]: BROADCAST flag
  // [1-15]: Must be zero (reserverd)
  uint16_t flags;

  // Client identifier unique to this client.  If used in one message, should
  // be used in all subsequet message.
  uint32_t ciaddr;

  // client IP address
  uint32_t yiaddr;

  // IP address of next server to use in bootstrap.  Returned in DHCPOFFER,
  // and DHCPACK by server
  uint32_t siaddr;

  // Relay agent IP address (used via relay agent)
  uint32_t giaddr;

  // Client hardware address
  uint8_t chaddr[16];

  // Optional server host name
  char sname[64];

  // Boot file name string, "generic" name or null in DHCPDISCOVER, fully
  // qualified directory name in DHCPOFFER
  char file[128];

  uint8_t options_signature[4];

  // Variable length options follow signature
};


// Each of the build functions below accepts a character buffer and a size
// pointer.  The value pointed to by size should be the maximum size of the
// buffer, and on return will be the actual size of the buffer used for the
// message.

// Populates the specified buffer with a DHCPDISCOVER request from the mac/host
// specified.  The source_mac must be a 6 byte ethernet address, and hostname
// is a NULL terminated string.  Returns -1 and logs to stderr on error.
int build_dhcp_discover(uint8_t* source_mac, const char* hostname,
                        uint8_t* buf, uint16_t* size);

int build_dhcp_request(uint32_t xid, uint8_t* source_mac, const char* hostname,
                       uint32_t requested_ip, uint32_t server_ip,
                       uint8_t* buf, uint16_t* size);

#endif
