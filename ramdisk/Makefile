CC = g++
CCFLAGS = -Wall -Werror -g
LIBS = -lfuse
OPTS = -D_FILE_OFFSET_BITS=64 -D__FreeBSD__=10 -DFUSE_USE_VERSION=26

SRC = node.cpp file.cpp dir.cpp ramdisk.cpp
OBJ = $(SRC:.cpp=.o)

.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CCFLAGS) $(OPTS) -c $<
# -c $< -o $@

all: ramdisk

test: file_test dir_test ramdisk_test
	./file_test && ./dir_test && ./ramdisk_test

libcommon.a: $(OBJ)
	ar rcs libcommon.a $(OBJ)

ramdisk: libcommon.a main.cpp
	$(CC) $(CCLFAGS) $(OPTS) -o ramdisk main.cpp -L. -lcommon $(LIBS)

file_test: libcommon.a file_test.cpp
	$(CC) $(CCFLAGS) $(OPTS) -o file_test file_test.cpp \
              -L. -lcommon $(LIBS)

dir_test: libcommon.a dir_test.cpp
	$(CC) $(CCFLAGS) $(OPTS) -o dir_test dir_test.cpp \
              -L. -lcommon $(LIBS)

ramdisk_test: libcommon.a ramdisk_test.cpp
	$(CC) $(CCFLAGS) $(OPTS) -o ramdisk_test ramdisk_test.cpp -L. \
              -lcommon $(LIBS)

clean: 
	-rm -rf *.o *.a *.core core ramdisk file_test dir_test ramdisk_test
