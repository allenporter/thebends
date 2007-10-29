// ether.h
// Author: Allen Porter <allen@thebends.org>
// Large portions of this code were written by Fred Eady of EDTP Electronics
//
// A library for capturing packets from the Easy Ethernet AVR board
// manufactured by EDTP Electronics
// See http://www.edtp.com/easyethavr_page.htm for more details.
//
// The Easy Ethernet AVR uses a RealtekRTL8019as, a common ethernet controller
// found in many AVR projects.  See the datasheet for all of the interesting
// details.
//
// Example usage:
//   void packet_received(unsigned char* buf, uint16_t len) {
//     ...
//   }
//   ...
//   char MAC_ADDR[6] = { 0, 0, 'E', 'D', 'T', 'P' };
//   ether_loop(MAC_ADDR, &packet_received);

#ifndef __ETHER_H__
#define __ETHER_H__

#include <inttypes.h>

typedef void (*packet_cb)(unsigned char* buf, uint16_t len);

void ether_set_addr(const uint8_t* mac_addr);

// Loops until a packet is received.  When received, invokes the packet
// callback function with a pointer to the buffer containing the packet, and
// its length, as arguments.
//
// Mac address should be a 6 byte buffer containting ASCII hex characeters
// suitable as a MAC address.
void ether_loop(packet_cb cb);

void ether_write(char* buf, uint16_t len);

#endif  // __ETHER_H__
