#ifndef __PACKET_FUNCTIONS_H__
#define __PACKET_FUNCTIONS_H__

#include "utunnel.h"

void ntoh_iphdr(struct ip * hdr);
int macaddr_set(struct ether_addr * addr, char * mac_address);
void write_external(uconfig * config, endpoint * dest,
		u_char * payload, u_int payload_len);
void write_loopback(uconfig * config, endpoint * src,
		u_char * payload, u_int payload_len);
void sniffer_init(uconfig * config, char * device,
	char * filter_string, pcap_handler callback);
void dump_ip_packet(struct ip * hdr);
void dump_udp_packet(struct udphdr * hdr);

#endif /* !__PACKET_FUNCTIONS_H__ */
