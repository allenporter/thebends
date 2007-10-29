MCU_TARGET = atmega8515
F_CPU = 1000000

CC = avr-gcc
CCFLAGS = -Wall -O2 -mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU)
OBJCOPY = avr-objcopy

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: blocking.hex interrupt.hex

%.elf: %.o
	$(CC) $(CCFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	rm -fr *.o *.elf *.lst *.hex
