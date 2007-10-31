CC=g++
CFLAGS=-Wall -Werror

.SUFFIXES: .cpp

all: public-ip

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libpmp.a: route.o
	ar rcs $@ $^

public-ip: public-ip.o libpmp.a
	$(CC) -o $@ $^ $(LDFLAGS) -lpmp -L.

clean:
	rm -f *.o *.a core public-ip
