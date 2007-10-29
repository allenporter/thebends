CC=g++
CCFLAGS=-Wall -Werror
LDFLAGS=-lgflags

all:  memedit

%.o: %.c
	$(CC) -c $(CCFLAGS) $< -o $@

memedit: memedit.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o memedit
