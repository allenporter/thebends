AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall -Werror
LDFLAGS = -framework CoreFoundation -framework CoreAudio

SRC = util.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: libyaudio.a devices sample

libyaudio.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

devices: devices.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -L. -lyaudio

sample: sample.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -L. -lyaudio

dist: clean
	REVISION=`svn info | grep "^Revision:" | cut -f 2 -d ' '` && \
	cd .. && tar zcvf ycoreaudio/libycoreaudio-$$REVISION.tgz ycoreaudio/*.cpp ycoreaudio/*.h ycoreaudio/Makefile ycoreaudio/README

clean:
	-rm -fr *.o *.a *.tgz core devices sample
