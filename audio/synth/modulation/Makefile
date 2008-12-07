.PHONY: clean

CC = g++
CFLAGS = -Wall -Werror -I../ -L. -g

SRC = modulation.cpp

OBJ = $(SRC:.cpp=.o)

all: libmodulation.a

.cpp.o:
	$(CC) -c $(CFLAGS) $<

libmodulation.a: $(OBJ)
	$(AR) rc $@ $(OBJ)

clean:
	-$(RM) *.o *.a *_test
