#!/bin/bash
#
# Helper script that runs avrdude to flash the specified device with the
# specified binary.  Variable can be overridden at runtime.
#
# Example usage:
# $ BINARY=example.hex ../flash.sh

: ${BINARY=""}                     # name of .hex file
: ${AVR="m32"}                     # Target device (atmega32)
: ${PROG="stk500v2"}               # Programmer type
: ${PORT="/dev/cu.USA19Hb11P1.1"}  # Serial port (Keyspan usb device)

if [ "$BINARY" == "" ]; then
  echo "Missing BINARY environment variable"
  exit 1
fi

CMD="avrdude -p $AVR -c $PROG -P $PORT -U flash:w:$BINARY"
echo "[*] $CMD"
eval $CMD
