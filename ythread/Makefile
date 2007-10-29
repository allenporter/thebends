# Author: Allen Porter <allen@thebends.org>

AR = ar
CC = g++ 
RANLIB = ranlib
INSTALL = install
CFLAGS = -O -Wall
INSTALLDIR = /usr/local
INSTALL_LIBDIR = $(INSTALLDIR)/lib
INSTALL_INCLUDEDIR = $(INSTALLDIR)/include/ythread

all: libythread.a

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libythread.a: thread.o mutex.o condvar.o threadpool.o callback.o
	$(AR) rc $@ thread.o mutex.o condvar.o threadpool.o callback.o
	$(RANLIB) $@

test: test.cpp libythread.a
	$(CC) $(CFLAGS) -o test test.cpp libythread.a -lpthread
	./test

install: libythread.a
	$(INSTALL) -p -m 755 libythread.a $(INSTALL_LIBDIR)
	mkdir -p $(INSTALL_INCLUDEDIR)
	$(INSTALL) -p -m 755 callback*.h thread.h mutex.h condvar.h threadpool.h $(INSTALL_INCLUDEDIR)

clean:
	-rm -f core *.o test libythread.a
