.PHONY: clean

CC = g++
CFLAGS = -Wall -Werror -I../ -L.

SRC = envelope.cpp

OBJ = $(SRC:.cpp=.o)

all: libenvelope.a envelope_test

.cpp.o:
	$(CC) -c $(CFLAGS) $<

libenvelope.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RM) $(OBJ)

envelope_test: libenvelope.a envelope_test.o
	$(CC) -o $@ $(CFLAGS) -lenvelope envelope_test.o

clean:
	-$(RM) *.o *.a *_test
