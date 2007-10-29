// net.c
// Author: Allen Porter <allen@thebends.org>
#include "net.h"
#include <avr/io.h>
#include <logging/logging.h>
#include <rtl8019as/ether.h>
#include <stdlib.h>
#include <string.h>
#include "arp.h"
#include "ip.h"
#include "util.h"

#define IP_STRING "10.0.1.180"
const char BROADCAST_ADDRESS[ETHER_ADDR_LEN] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
const char MAC_ADDRESS[ETHER_ADDR_LEN] = { 'a', 'l', 'l', 'e', 'n', '1' };
uint32_t IP;

static void read_packet(unsigned char* buf, uint16_t len) {
  if (len < sizeof(struct ether_header)) {
    lput_str("Ethernet header was too short");
    return;
  }
  struct ether_header* eth = (struct ether_header*)buf;
  if (memcmp(eth->dest, MAC_ADDRESS, ETHER_ADDR_LEN) != 0 &&
      memcmp(eth->dest, BROADCAST_ADDRESS, ETHER_ADDR_LEN) != 0) {
    lput_str("Ignoring packet not meant for me");
    return;
  } 
  uint16_t payload_len = len - sizeof(struct ether_header);
  unsigned char* payload = (unsigned char*)(eth + 1);
  ntohs(&eth->type);
  switch (eth->type) {
    case ETHERTYPE_IP:
      if (payload_len < sizeof(struct ip_header)) {
        lput_str("IP header was too short");
        return;
      }
      unsigned char* ip_payload = payload + sizeof(struct ip_header);
      uint16_t ip_payload_len = payload_len - sizeof(struct ip_header);
      handle_ip((struct ip_header*)payload, ip_payload, ip_payload_len);
      break;
    case ETHERTYPE_ARP:
      if (payload_len < sizeof(struct arp_header)) {
        lput_str("ARP header was too short");
        return;
      }
      handle_arp((struct arp_header*)payload);
      break;
    default:
      lput_str("Unknown ETHERTYPE:");
      lput16_hex(eth->type);
      lput_str("\r\n");
      break;
  }
}

void write_packet(unsigned char* ether_addr, uint16_t ether_type,
                  unsigned char* buf, uint16_t len) {
  uint16_t packet_length = len + sizeof(struct ether_header);
  unsigned char* packet = malloc(packet_length);
  struct ether_header* eth = (struct ether_header*)packet;
  memcpy(eth->dest, ether_addr, ETHER_ADDR_LEN);
  memcpy(eth->source, MAC_ADDRESS, ETHER_ADDR_LEN);
  eth->type = ether_type;
  ntohs(&eth->type);
  memcpy(eth + 1, buf, len);

  lput_str("OUT: ");
  lput16_hex(packet_length);
  lput_str(" bytes\r\n");

  lput_buffer(packet, packet_length);

  ether_write(packet, packet_length); 
  lput_str("wrote\r\n.\r\n");
  free(buf);
}

int main(void) {
  logging_init();
  if (inet_aton(IP_STRING, &IP) == -1) {
    lput_str("[*] failed to parse IP string\r\n");
    return 0;
  }
  lput_str("[*] my ip: 0x");
  lput16_hex((IP >> 16) & 0xffff);
  lput16_hex(IP & 0xffff);
  lput_str("\r\n[*] net initialized;\r\n");
  ether_set_addr(MAC_ADDRESS);
  ether_loop(&read_packet);
  return 0;
}
