MCU_TARGET = atmega8515
CPU = 1000000

CC = avr-gcc
CCFLAGS = -Wall -O2 -mmcu=$(MCU_TARGET) -DF_CPU=$(CPU)
OBJCOPY = avr-objcopy

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: count.hex

%.elf: %.o
	$(CC) $(CCFLAGS) -o $@ $^

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	rm -fr *.o *.elf *.lst *.hex
