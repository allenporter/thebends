CC=g++
CFLAGS=-Wall -Werror -g
LDFLAGS=-lythread
TOOL_LDFLAGS=$(LDFLAGS) -lgflags -lyhttp

all: libbtunnel.a register_service tcp_proxy bproxy bproxy_serve

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libbtunnel.a: tcpserver.o util.o registration.o tunnel.o service.o browse.o
	$(AR) rc $@ $^

%_test: %_test.o libbtunnel.a
	$(CC) $(CFLAGS) -g -o $@ $^ $(LDFLAGS)

register_service: register_service.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

tcp_proxy: tcp_proxy.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

bproxy: bproxy.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

bproxy_server: bproxy_server.o libbtunnel.a
	$(CC) -o $@ $^ $(TOOL_LDFLAGS)

clean:
	rm -f *.o *.a core *_capture *_test tcp_proxy register_service bproxy bproxy_server
