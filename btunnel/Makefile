CC=g++
CFLAGS=-Wall -Werror
LDFLAGS=-lythread
TOOL_LDFLAGS=$(LDFLAGS) -lgflags

all: libbtunnel.a multicast_capture udp_capture register_service

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libbtunnel.a: socket.o udp.o multicast.o listener.o writer.o tunnel.o \
              util.o packer.o registration.o
	$(AR) rc $@ $^

%_capture: %_capture.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)


%_test: %_test.o libbtunnel.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

register_service: register_service.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS) -framework CoreFoundation


test: multicast_test
	./multicast_test

clean:
	rm -f *.o *.a core *_capture *_test
