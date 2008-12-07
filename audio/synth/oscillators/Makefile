.PHONY: clean

CC = g++
CFLAGS = -Wall -Werror -I../ -L.

SRC = oscillator.cpp sine.cpp square.cpp triangle.cpp sawtooth.cpp combiner.cpp

OBJ = $(SRC:.cpp=.o)

all: liboscillators.a oscillator_test

.cpp.o:
	$(CC) -c $(CFLAGS) $<

liboscillators.a: $(OBJ)
	$(AR) rc $@ $(OBJ)

oscillator_test: liboscillators.a oscillator_test.o
	$(CC) -o $@ $(CFLAGS) -loscillators oscillator_test.o

clean:
	-$(RM) *.o *.a oscillator_test
