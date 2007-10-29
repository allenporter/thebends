// ip.c
// Author: Allen Porter <allen@thebends.org>
#include "ip.h"
#include <avr/io.h>
#include <logging/logging.h>
#include "net.h"
#include "util.h"

void handle_ip(struct ip_header* ip, unsigned char* payload,
               uint16_t payload_len) {
  ntohs(&ip->ip_len);
  ntohs(&ip->ip_id);
  ntohs(&ip->ip_off);
  ntohs(&ip->ip_sum);
  ntohl(&ip->ip_src);
  ntohl(&ip->ip_dst);
  if (ip->ip_vhl != 0x45 || ip->ip_dst != IP) {
    lput_str("Ignoring packet\r\n");
    return;
  }
  uint16_t len = (payload_len + sizeof(struct ip_header));
  if (ip->ip_len > len) {
    lput_str("Packet length is invalid");
    return;
  }
  payload_len = len - sizeof(struct ip_header);
  // TODO: Support packet fragments
  if (ip->ip_off != 0) {
    lput_str("Ignoring packet fragment; unsupported");
    return;
  }
  lput_str("Got ip packet, protocol=");
  switch (ip->ip_p) {
    case IPPROTO_ICMP:
      lput_str("ICMP!");
      if (payload_len < ) {
      }
      break;
    case IPPROTO_TCP:
      lput_str("TCP");
      break;
    case IPPROTO_UDP:
      lput_str("UDP");
      break;
    default:
      lput_str("Unknown IP Protocol: ");
      lputc_hex(ip->ip_p);
      break;
  }
  lput_str("\r\n");
}

