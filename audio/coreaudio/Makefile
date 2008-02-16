AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror
LDFLAGS = -framework CoreFoundation -framework CoreAudio

SRC =
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: devices

devices: devices.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	-rm -fr *.o *.a *.tgz core devices
