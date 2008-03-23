AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror -I/usr/local/include -g
LDFLAGS = -L/usr/local/lib -lyhttp -lynet -lythread
INSTALL = install
INSTALLDIR = /usr/local
INSTALL_LIBDIR = $(INSTALLDIR)/lib
INSTALL_INCLUDEDIR = $(INSTALLDIR)/include/ypeer

SRC = response.cpp bencode.cpp peers.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: libypeer.a find_peers

libypeer.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

install: libypeer.a
	$(INSTALL) -p -m 755 libypeer.a $(INSTALL_LIBDIR)
	mkdir -p $(INSTALL_INCLUDEDIR)
	$(INSTALL) -p -m 755 *.h $(INSTALL_INCLUDEDIR)

find_peers: find_peers.o libypeer.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

response_test: response_test.o libypeer.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

test: response_test
	./response_test


dist: clean
	REVISION=`svn info | grep "^Revision:" | cut -f 2 -d ' '` && \
	cd .. && tar zcvf ypeer/ypeer-$$REVISION.tgz ypeer/*.cpp ypeer/*.h ypeer/Makefile ypeer/README

clean:
	-rm -fr *.o *.a *.tgz core *_test find_peers
