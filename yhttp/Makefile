AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall
INSTALL = install
INSTALLDIR = /usr/local
INSTALL_LIBDIR = $(INSTALLDIR)/lib
INSTALL_INCLUDEDIR = $(INSTALLDIR)/include/yhttp

SRC = url.cpp util.cpp select.cpp reader.cpp writer.cpp httpserver.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: libyhttp.a

libyhttp.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

simple_server: simple_server.cpp libyhttp.a
	$(CC) $(CCFLAGS) -o simple_server simple_server.cpp -L. -lyhttp  -lythread

test: util_test url_test

util_test: util_test.cpp libyhttp.a
	$(CC) $(CCFLAGS) -o util_test util_test.cpp libyhttp.a
	./util_test

url_test: url_test.cpp libyhttp.a
	$(CC) $(CCFLAGS) -o url_test url_test.cpp libyhttp.a
	./url_test

install: libyhttp.a
	$(INSTALL) -p -m 755 libyhttp.a $(INSTALL_LIBDIR)
	mkdir -p $(INSTALL_INCLUDEDIR)
	$(INSTALL) -p -m 755 *.h $(INSTALL_INCLUDEDIR)

clean:
	-rm -fr *.o *.a core simple_server util_test url_test
