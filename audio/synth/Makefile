ECHO = echo
INSTALL_DIR = /usr/local
CC = g++
CFLAGS = -Wall -Werror -I../ -L.
.PHONY: all clean test

SRC = oscillator.cpp combiner.cpp envelope.cpp controller.cpp modulation.cpp
OBJ = $(SRC:.cpp=.o)

all: $(SUBDIRS) libsynth.a

.cpp.o:
	$(CC) -c $(CFLAGS) $<

libsynth.a: $(OBJ)
	$(AR) rc $@ $(OBJ)

oscillator_test: libsynth.a oscillator_test.o
	$(CC) -o $@ $(CFLAGS) -lsynth oscillator_test.o

envelope_test: libsynth.a envelope_test.o
	$(CC) -o $@ $(CFLAGS) -lsynth envelope_test.o

test: oscillator_test envelope_test
	./oscillator_test
	./envelope_test

install: libsynth.a
	cp libsynth.a /usr/local/lib/
	mkdir -p /usr/local/include/synth/
	cp -rp *.h /usr/local/include/synth/

clean:
	-$(RM) *.o *.a __* *_test
