#include "arp.h"
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"
#include "net.h"

// Maps protocol addresses to hardware addresses
struct list {
  struct list* next;
  uint32_t ar_pa;                 // ip address
  uint8_t ar_ha[ETHER_ADDR_LEN];  // hardware address
};

// List of ARP entries. This list grows without bound.
static struct list* head = NULL;

static struct list* find(uint32_t ar_pa) {
  struct list* cur = head;
  while (cur != NULL && (ar_pa != cur->ar_pa)) {
    cur = cur->next;
  }
  return cur;
}

static int update(uint32_t ar_pa, uint8_t* ar_ha) {
  struct list* entry = find(ar_pa);
  if (entry != NULL) {
    memcpy(entry->ar_ha, ar_ha, ETHER_ADDR_LEN);
    return 1;
  }    
  return 0;
}

static void insert(uint32_t ar_pa, uint8_t* ar_ha) {
  struct list* entry = malloc(sizeof(struct list));
  entry->ar_pa = ar_pa;
  memcpy(entry->ar_ha, ar_ha, ETHER_ADDR_LEN);
  entry->next = head;
  head = entry;
}

void handle_arp(struct arp_header* arp) {
  ntohs(&arp->ar_hrd);
  ntohs(&arp->ar_pro);
  ntohs(&arp->ar_op);
  ntohl(&arp->ar_spa);
  ntohl(&arp->ar_tpa);
  if (arp->ar_hrd != ARPHRD_ETHER ||
      arp->ar_pro != ETHERTYPE_IP ||
      arp->ar_hln != ETHER_ADDR_LEN ||
      arp->ar_pln != IP_ADDR_LEN) {
    return;
  }
  int merge = update(arp->ar_spa, arp->ar_sha);
  if (IP == arp->ar_tpa) {
    if (!merge) {
      insert(arp->ar_spa, arp->ar_sha);
    }
    if (arp->ar_op == ARPOP_REQUEST) {
      arp->ar_op = ARPOP_REPLY;
      memcpy(arp->ar_tha, arp->ar_sha, ETHER_ADDR_LEN);
      arp->ar_tpa = arp->ar_spa;
      memcpy(arp->ar_sha, MAC_ADDRESS, ETHER_ADDR_LEN);
      arp->ar_spa = IP;
      ntohs(&arp->ar_hrd);
      ntohs(&arp->ar_pro);
      ntohs(&arp->ar_op);
      ntohl(&arp->ar_spa);
      ntohl(&arp->ar_tpa);
      write_packet(arp->ar_tha, ETHERTYPE_ARP, (unsigned char*)arp,
                   sizeof(struct arp_header));
    }
  }
}
