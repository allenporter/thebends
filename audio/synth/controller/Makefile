.PHONY: clean

CC = g++
CFLAGS = -Wall -Werror -I../

SRC = controller.cpp

OBJ = $(SRC:.cpp=.o)

all: libcontroller.a

.cpp.o:
	$(CC) -c $(CFLAGS) $<

libcontroller.a: $(OBJ)
	$(AR) rc $@ $(OBJ)
	$(RM) $(OBJ)

clean:
	-$(RM) *.o *.a
