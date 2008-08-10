CC = g++
CCFLAGS = -Wall -Werror -g
LDFLAGS =

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: modify_reg

modify_reg: modify_reg.o mmap.o
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	-rm -fr *.o modify_eip
