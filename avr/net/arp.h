// arp.h
// Author: Allen Porter <allen@thebends.org>
//
// See RFC 826 for more details
//
#ifndef __ARP_H__
#define __ARP_H__

#include <inttypes.h>
#include "net.h"

struct arp_header {
        uint16_t ar_hrd;         /* format of hardware address */
#define ARPHRD_ETHER    1        /* ethernet hardware format */
        uint16_t ar_pro;         /* format of protocol address */
        uint8_t  ar_hln;         /* length of hardware address */
        uint8_t  ar_pln;         /* length of protocol address */
        uint16_t ar_op;          /* one of: */
#define ARPOP_REQUEST   1        /* request to resolve address */
#define ARPOP_REPLY     2        /* response to previous request */
        uint8_t  ar_sha[ETHER_ADDR_LEN];  /* sender hardware address */
        uint32_t ar_spa;                  /* sender protocol address */
        uint8_t  ar_tha[ETHER_ADDR_LEN];  /* target hardware address */
        uint32_t ar_tpa;                  /* target protocol address */
};

void handle_arp(struct arp_header* arp);

#endif  // __ARP_H__
