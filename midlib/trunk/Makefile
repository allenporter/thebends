CC = g++
AR = ar
CFLAGS = -Wall

SRC = buffer.cc chunk.cc midi.cc event.cc
OBJ = $(SRC:.cc=.o)
OUT = libmid.a

.SUFFIXES: .cc

default: $(OUT)

.cc.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(AR) rcs $(OUT) $(OBJ)

midparse: midparse.cc libmid.a
	$(CC) $(CFLAGS) -o midparse midparse.cc -L. -lmid

clean:
	rm -f $(OBJ) $(OUT) midparse Makefile.bak
# DO NOT DELETE

buffer.o: buffer.h /usr/include/string.h /usr/include/_types.h
buffer.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
buffer.o: /usr/include/machine/_types.h /usr/include/ppc/_types.h
chunk.o: chunk.h /usr/include/string.h /usr/include/_types.h
chunk.o: /usr/include/sys/_types.h /usr/include/sys/cdefs.h
chunk.o: /usr/include/machine/_types.h /usr/include/ppc/_types.h buffer.h
midi.o: midi.h buffer.h chunk.h
