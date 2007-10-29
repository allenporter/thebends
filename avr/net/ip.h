// ip.h
// Author: Allen Porter <allen@thebends.org>

#ifndef __IP_H__
#define __IP_H__

#include <inttypes.h>

struct ip_header {
        uint8_t  ip_vhl;                 /* version << 4 | header length >> 2 */
        uint8_t  ip_tos;                 /* type of service */
        uint16_t ip_len;                 /* total length */
        uint16_t ip_id;                  /* identification */
        uint16_t ip_off;                 /* fragment offset field */
#define IP_RF 0x8000                     /* reserved fragment flag */
#define IP_DF 0x4000                     /* dont fragment flag */
#define IP_MF 0x2000                     /* more fragments flag */
#define IP_OFFMASK 0x1fff                /* mask for fragmenting bits */
        uint8_t  ip_ttl;                 /* time to live */
        uint8_t  ip_p;                   /* protocol */
#define IPPROTO_ICMP 1
#define IPPROTO_TCP 6
#define IPPROTO_UDP 17
        uint16_t ip_sum;                 /* checksum */
        uint32_t ip_src,ip_dst;  /* source and dest address */
};

void handle_ip(struct ip_header* ip, unsigned char* payload,
               uint16_t payload_len);

#endif  // __IP_H__
