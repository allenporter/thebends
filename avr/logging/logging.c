// logging.c
// Author: Allen Porter <allen@thebends.org>

#include "logging.h"
#include <avr/interrupt.h>
#include <avr/signal.h>

#define BUFFER_SIZE 32  // must be power of 2
#define BUFFER_MASK (BUFFER_SIZE - 1)

// Circular output buffer
static char buffer[BUFFER_SIZE];
static uint8_t head = 0;
static uint8_t tail = 0;

void delay(unsigned int delay_ms) {
  OCR1AH = 0x1C;
  OCR1AL = 0xCC;
  TCCR1B = 0x00;          // Stop Timer1
  unsigned int i;
  for (i = 0; i < delay_ms; ++i) {
    TCCR1B = 0x00;          // Stop Timer1
    TCNT1H = 0x00;          // Clear Timer1
    TCNT1L = 0x00;
    TCCR1B = 0x09;          // Start Timer1 with clk/1
    while(!(TIFR & 0x10));
    TIFR |= 0x10;
  }
}

void logging_init() {
  cli();  // disable interrupts
  unsigned int baud = F_CPU / 16 / BAUD - 1;
  UBRRH = (unsigned char)(baud>>8);
  UBRRL = (unsigned char)baud;
  UCSRB = _BV(TXEN);
  // 8N1
  UCSRC = _BV(URSEL) | (0<<USBS) | (3<<UCSZ0);
  sei();  // enable interrupts
}

SIGNAL(SIG_UART_TRANS) {
  if (head != tail) {
    tail = (tail + 1) & BUFFER_MASK;
    UDR = buffer[tail];
  } else {
    // buffer is empty, disable SIG_UART_TRANS
    UCSRB &= ~(_BV(TXCIE));
  }
}

// Returns an ASCII hex character for the specified value.  The value must be
// less than 16
static char hex(uint8_t value) {
  if (value > 16) {
    return 'X';
  } else if (value > 9) {
    return (value - 10) + 'a';
  } else {
    return '0' + value;
  }
}

/*
void lputc(char c) {
  if ((UCSRB & _BV(TXCIE)) != 0) {
    int8_t tmphead;
    tmphead = (head + 1) & BUFFER_MASK;
    while (tmphead == tail) {
      // Doesn't seem to yield for SIG_UART_TRANS interrupts unless we specify
      // some delay.  TODO: Find a better way to do this, or explicitly yield.
      delay(1);
    }
    buffer[tmphead] = c;
    head = tmphead;
  } else {
    UDR = c;
    UCSRB |= _BV(TXCIE);
  }
}
*/

// TODO: Either fix the bugs in the async version or scrap this whole library
// and use <stdio.h>

void lputc(char c) {
  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
}

void lputc_hex(uint8_t value) {
  // most significant 4 bits first
  lputc(hex((value & 0xf0) >> 4));
  lputc(hex((value & 0x0f)));
}

void lput16_hex(uint16_t value) {
  // most significant byte first
  uint8_t b = (uint8_t)((value & 0xff00) >> 8);
  lputc_hex(b);
  uint8_t a = (uint8_t)(value & 0x00ff);
  lputc_hex(a);
}

void lput_str(const char* str) {
  const char* c = str;
  while (*c != '\0') {
    lputc(*c);
    c++;
  }
}

void lput_buffer(const char* buf, int len) {
  int i;
  lput_str("[ ");
  for (i = 0; i < len; ++i) {
    char c = buf[i];
    if (i != 0 && (i % 24) == 0) {
      lput_str("\r\n: ");
    }
    lputc_hex(c);
    lputc(' ');
  }
  lput_str("]\r\n");
}
