CC = gcc
CCFLAGS = -Wall -Werror -g
LDFLAGS =

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: modify_eip

modify_eip: modify_eip.o mmap.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	-rm -fr *.o modify_eip
