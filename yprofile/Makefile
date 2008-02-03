AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror -I/usr/local/include
LDFLAGS =
INSTALL = install
INSTALLDIR = /usr/local
INSTALL_LIBDIR = $(INSTALLDIR)/lib
INSTALL_INCLUDEDIR = $(INSTALLDIR)/include/yprofile

SRC = profile.cpp stack.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: libyprofile.a

libyprofile.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

install: libyprofile.a
	$(INSTALL) -p -m 755 libyprofile.a $(INSTALL_LIBDIR)
	mkdir -p $(INSTALL_INCLUDEDIR)
	$(INSTALL) -p -m 755 *.h $(INSTALL_INCLUDEDIR)

example: example.o libyprofile.a
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

stack_test: stack_test.o libyprofile.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

test: stack_test
	./stack_test

#dist: clean
#	REVISION=`svn info | grep "^Revision:" | cut -f 2 -d ' '` && \
#	cd .. && tar zcvf yprofile/yprofile-$$REVISION.tgz yprofile/*.cpp yprofile/*.h yprofile/Makefile yprofile/README

clean:
	-rm -fr *.o *.a *.tgz core *_test example
