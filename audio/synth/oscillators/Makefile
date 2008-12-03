.PHONY: clean

CC = g++
CFLAGS = -Wall -Werror -I../

SRC = oscillator.cpp sine.cpp square.cpp combiner.cpp

OBJ = $(SRC:.cpp=.o)

all: liboscillators.a

.cpp.o:
	$(CC) -c $(CFLAGS) $<

liboscillators.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RM) $(OBJ)

clean:
	-$(RM) *.o *.a
