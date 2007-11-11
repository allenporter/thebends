CC=g++
CFLAGS=-Wall -Werror
LDFLAGS=-lythread
TOOL_LDFLAGS=$(LDFLAGS) -lgflags -lyhttp

all: libbtunnel.a udp_capture register_service tcp_proxy \
     bproxy

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libbtunnel.a: socket.o udp.o listener.o writer.o tcpserver.o \
              util.o packer.o registration.o tunnel.o
	$(AR) rc $@ $^

%_capture: %_capture.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)


%_test: %_test.o libbtunnel.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

register_service: register_service.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

tcp_proxy: tcp_proxy.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

bproxy: bproxy.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

clean:
	rm -f *.o *.a core *_capture *_test tcp_proxy register_service bproxy
