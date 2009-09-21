AR = ar
CC = g++
RANLIB = ranlib
CCFLAGS = -Wall # -Werror
LDFLAGS = -framework CoreFoundation -framework CoreAudio -framework AudioToolbox

SRC = util.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) -c $<

all: libyaudio.a devices sample queue

libyaudio.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RANLIB) $@

devices: devices.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -L. -lyaudio

sample: sample.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) -L. -lyaudio

queue: queue.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

dist: clean
	REVISION=`svn info | grep "^Revision:" | cut -f 2 -d ' '` && \
	cd .. && tar zcvf ycoreaudio/libycoreaudio-$$REVISION.tgz ycoreaudio/*.cpp ycoreaudio/*.h ycoreaudio/Makefile ycoreaudio/README

clean:
	-rm -fr *.o *.a *.tgz core devices sample
