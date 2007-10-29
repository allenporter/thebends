#MCU_TARGET = atmega8515
MCU_TARGET = atmega32
F_CPU = 8000000UL  # 8MHz
BAUD = 38400

CC = avr-gcc
AR = avr-ar
CCFLAGS = -Wall -O2 -mmcu=$(MCU_TARGET) -DF_CPU=$(F_CPU) -DBAUD=$(BAUD)

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: logging.a

%.a: %.o
	$(AR) rcs lib$@ $^

clean:
	rm -fr *.o *.a
