CC=g++
CFLAGS=-Wall -Werror
LDFLAGS=-lgflags

.SUFFIXES: .cpp

all: public-ip create-map

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libpmp.a: route.o op.o util.o
	ar rcs $@ $^

public-ip: public-ip.o libpmp.a
	$(CC) -o $@ $^ $(LDFLAGS) -lpmp -L.

create-map: create-map.o libpmp.a
	$(CC) -o $@ $^ $(LDFLAGS) -lpmp -L.

clean:
	rm -f *.o *.a core public-ip create-map
