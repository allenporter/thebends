// logging.h
// Author: Allen Porter <allen@thebends.org>
//
// A library for Atmel AVR microcontrollers that logs arbitary data to a serial
// port.  This can be useful for error logging or debugging.  This library
// uses async writes with a small circular buffer, but may block momentarily
// if the buffer is small.  The techniques used here are similar to most
// async USART examples, but packaged in a library.  This library expects to
// be the only code using the serial port.
//
// Example usage:
//
//    logging_init();
//    lput_str("Starting...");
//    ...
//    lput_str("Value is: ");
//    lputc_hex(value);
//    lprint_str("\r\n");

#include <inttypes.h>

// Must be called to initialize the logging library prior to making any calls
// in this library.  The serial port runs at the baud rate defined in BAUD (see // Makefile) using 8N1.
void logging_init();

// Outputs the specified byte value as a two ASCII hex characters.
void lputc_hex(uint8_t value);

// Outputs the specified two byte value as four ASCII hex characeters.
void lput16_hex(uint16_t val);

// Outputs the specified NULL terminated string.
void lput_str(const char *str);

// Outputs the specified buffer of data as ASCII hex characters.
void lput_buffer(const char *buf, int len);
