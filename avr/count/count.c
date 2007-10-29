#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/delay.h>

int main(void) {
  DDRC = 0xff;  // output on all pins

  for (;;) {
    int16_t i;
    for (i = 0; i < 255; i++) {
      PORTC = ~i;
      _delay_ms(10);
    }
  }
  return 0;
}
