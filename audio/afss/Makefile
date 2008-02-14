AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror
LDFLAGS =

SRC =
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: stream

stream: stream.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	-rm -fr *.o *.a *.tgz core stream
