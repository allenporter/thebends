MCU_TARGET = atmega32
F_CPU = 8000000UL # 8MHz

CC = avr-gcc
CCFLAGS = -Wall -O2 -mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU) -I../
LDFLAGS = -L../logging -llogging -L../rtl8019as -lether
OBJCOPY = avr-objcopy

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: net.hex

%.elf: arp.o net.o ip.o util.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LDFLAGS)

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

clean:
	rm -fr *.o *.elf *.lst *.hex
