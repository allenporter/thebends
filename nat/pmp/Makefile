AR=ar
RANLIB=ranlib
CC=g++
CFLAGS=-Wall -Werror
LDFLAGS=-lgflags
INSTALL=install
INSTALLDIR=/usr/local
INSTALL_LIBDIR=$(INSTALLDIR)/lib
INSTALL_INCLUDEDIR=$(INSTALLDIR)/include/natpmp

.SUFFIXES: .cpp

all: public-ip create-map

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libnatpmp.a: route.o op.o util.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

public-ip: public-ip.o libnatpmp.a
	$(CC) -o $@ $^ $(LDFLAGS) -lnatpmp -L.

create-map: create-map.o libnatpmp.a
	$(CC) -o $@ $^ $(LDFLAGS) -lnatpmp -L.

install: libnatpmp.a
	$(INSTALL) -p -m 755 libnatpmp.a $(INSTALL_LIBDIR)
	mkdir -p $(INSTALL_INCLUDEDIR)
	$(INSTALL) -p -m 755 *.h $(INSTALL_INCLUDEDIR)

clean:
	rm -f *.o *.a core public-ip create-map
