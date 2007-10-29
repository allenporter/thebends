MCU_TARGET = atmega32
F_CPU = 8000000UL

CC = avr-gcc
AR = avr-ar
CCFLAGS = -Wall -O2 -mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU) -I../

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: ether.a

%.a: ether.o
	$(AR) rcs lib$@ $^

clean:
	rm -fr *.o *.a
