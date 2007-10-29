CC=g++
CFLAGS=-Wall -Werror
LDFLAGS=-lythread
TOOL_LDFLAGS=$(LDFLAGS) -lgflags

all: libbtunnel.a multicast_capture udp_capture

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libbtunnel.a: socket.o udp.o multicast.o listener.o writer.o tunnel.o \
              util.o packer.o
	$(AR) rc $@ $^

%_capture: %_capture.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)


%_test: %_test.o libbtunnel.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

test: multicast_test
	./multicast_test

clean:
	rm -f *.o *.a core *_capture *_test
