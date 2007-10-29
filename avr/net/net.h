// net.h
// Author: Allen Porter <allen@thebends.org>
#ifndef __NET_H__
#define __NET_H__

#include <inttypes.h>

#define ETHER_ADDR_LEN 6
#define IP_ADDR_LEN 4

extern const char BROADCAST_ADDRESS[ETHER_ADDR_LEN];
extern const char MAC_ADDRESS[ETHER_ADDR_LEN];
extern uint32_t IP;

// Structure of a 10Mb/s Ethernet header
struct ether_header {
  unsigned char dest[ETHER_ADDR_LEN];
  unsigned char source[ETHER_ADDR_LEN];
  uint16_t type;
#define ETHERTYPE_IP 0x0800
#define ETHERTYPE_ARP 0x0806
};

void write_packet(unsigned char* ether_addr, uint16_t ether_type,
                  unsigned char* buf, uint16_t len);

#endif  // __NET_H__
