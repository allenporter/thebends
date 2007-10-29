// interrupt.c
// Author: Allen Porter <allen@thebends.org>
//
// Interrupt driven serial I/O, dumps the receive buffer every second.  The
// delay is mostly to show that the buffering is working.
//
// Written with help from lots of random snippets around

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/sleep.h>
#include <avr/delay.h>

#define BAUD 2400
#define BUFSIZ 255

char receive[BUFSIZ];
uint8_t receive_head = 0;
uint8_t receive_tail = 0;

char transmit[BUFSIZ];
uint8_t transmit_head = 0;
uint8_t transmit_tail = 0;

#define BUFFER_LENGTH(name) \
  ((name##_tail > name##_head) ? name##_tail - name##_head  \
                               : name##_head - name##_tail)
#define ADVANCE(name) name = (name + 1) % BUFSIZ;

void init(unsigned int baud) {
  unsigned int ubrr = (F_CPU / (16 * baud)) - 1;
  UBRRH = (unsigned char)(ubrr >> 8);
  UBRRL = (unsigned char)ubrr;
  UCSRB = _BV(RXEN) | _BV(TXEN) | _BV(RXCIE);
  UCSRC = _BV(URSEL) | (0<<USBS) | (3<<UCSZ0);
}

// _delay_ms can only sleep for a little more than 200ms
void sleep(int seconds) {
  int stop = seconds * 5;
  int i;
  for (i = 0; i < stop; i++) {
    _delay_ms(200);
  }
}

SIGNAL(SIG_UART_RECV) {
  if (receive_tail + 1 % BUFSIZ== receive_head) {
    return;  // full!
  }
  receive[receive_tail] = UDR;
  ADVANCE(receive_tail);
}

SIGNAL(SIG_UART_TRANS) {
  if (transmit_head != transmit_tail) {
    UDR = transmit[transmit_head];  
    ADVANCE(transmit_head);
  } else {
    // buffer is empty, disable SIG_UART_TRANS
    UCSRB &= ~(_BV(TXCIE));
  }
}

void writec(char data) {
  if ((UCSRB & _BV(TXCIE)) != 0) {
    if (transmit_tail + 1 % BUFSIZ == transmit_head) {
      return;  // full
    }
    transmit[transmit_tail] = data;
    ADVANCE(transmit_tail);
  } else {
    UDR = data;
    UCSRB |= _BV(TXCIE);
  }
}

void dump_buffer() {
  while (receive_head != receive_tail) {
    writec(receive[receive_head]);
    ADVANCE(receive_head);
  }
}

int main(void) {
  cli();  // disable interrupts
  init(BAUD);
  sei();  // enable interrupts

  DDRC = 0xff;

  for (;;) {
    sleep(1);
    dump_buffer();
  }

  return 0;
}
