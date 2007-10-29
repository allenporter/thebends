// blocking.c
// Author: Allen Porter <allen@thebends.org>
//
// Simple AVR serial echo program.  This program increments a counter on PORTC
// for each byte read, and echos back ascii integers when typed.

#include <inttypes.h>
#include <avr/io.h>

#define BAUD 2400

void init(unsigned int baud) {
  unsigned int ubrr = (F_CPU / (16 * baud)) - 1;
  UBRRH = (unsigned char)(ubrr >> 8);
  UBRRL = (unsigned char)ubrr;
  UCSRB = (1<<RXEN) | (1<<TXEN);
  UCSRC = (1<<URSEL) | (0<<USBS) | (3<<UCSZ0);
}

void transmit(unsigned int data) {
  while (!(UCSRA & (1<<UDRE))) {
    // nothing
  }
  UDR = data;
}

unsigned char receive() {
  while (!(UCSRA & (1<<RXC))) {
    // nothing
  }
  return UDR;
}

int main(void) {
  uint8_t i = 0;
  init(BAUD);
  DDRC = 0xff;
  while (1) {
    i = (i + 1) & (0xff);
    PORTC = ~i;
    char c = receive();
    if (c >= '0' && c <= '9') { 
      transmit(c);
    }
  }
  return 0;
}
