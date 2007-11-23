AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror
INSTALL = install
INSTALLDIR = /usr/local
INSTALL_LIBDIR = $(INSTALLDIR)/lib
INSTALL_INCLUDEDIR = $(INSTALLDIR)/include/ynet

SRC = select.cpp tcp_server.cpp tunnel.cpp
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

clean:
	-rm -fr *.o *.a core
