// packet_dump.c
// Author: Allen Porter <allen@thebends.org>
//
// Simple example program that dumps packets over a serial port, demonstrating
// use of the rtl8019as and logging libraries.  See each libraries header files
// for more detailed information.
//
// The ethernet library implementation does not attempt to do any parsing of
// packets and returns packets destined for all MAC addresses.

#include <avr/io.h>
#include <logging/logging.h>
#include <rtl8019as/ether.h>

void get_packet(unsigned char* buf, uint16_t len) {
  lput_str("[*] packet received\r\n");
  lput_buffer(buf, len);
  lput_str("[*] end of packet\r\n");
}

int main(void) {
  logging_init();
  lput_str("[*] packet_dump initialized; waiting for packet\r\n");
  ether_loop(&get_packet);
  return 0;
}
