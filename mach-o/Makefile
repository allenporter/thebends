CC = g++
CCFLAGS = -Wall -Werror -g
LDFLAGS =

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: modify_reg append_segment

modify_reg: modify_reg.o mmap.o
	$(CC) -o $@ $^ $(LDFLAGS)

append_segment: append_segment.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	-rm -fr *.o modify_eip
