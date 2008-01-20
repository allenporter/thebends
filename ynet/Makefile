AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror -I/usr/local/include
LDFLAGS = -lythread
INSTALL = install
INSTALLDIR = /usr/local
INSTALL_LIBDIR = $(INSTALLDIR)/lib
INSTALL_INCLUDEDIR = $(INSTALLDIR)/include/ynet

SRC = select.cpp tcp_server.cpp tcp_client.cpp tunnel.cpp buffer.cpp \
      buffered_reader.cpp buffered_writer.cpp util.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: libynet.a

libynet.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

install: libynet.a
	$(INSTALL) -p -m 755 libynet.a $(INSTALL_LIBDIR)
	mkdir -p $(INSTALL_INCLUDEDIR)
	$(INSTALL) -p -m 755 *.h $(INSTALL_INCLUDEDIR)

buffer_test: buffer_test.o libynet.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

buffered_writer_test: buffered_writer_test.o libynet.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

tcp_proxy: tcp_proxy.o libynet.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -lgflags

test: buffer_test buffered_writer_test
	./buffer_test
	./buffered_writer_test

dist: clean
	REVISION=`svn info | grep "^Revision:" | cut -f 2 -d ' '` && \
	cd .. && tar zcvf ynet/ynet-$$REVISION.tgz ynet/*.cpp ynet/*.h ynet/Makefile ynet/README

clean:
	-rm -fr *.o *.a *.tgz core *_test tcp_proxy
