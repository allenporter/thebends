CC=g++
CFLAGS=-Wall -Werror -g
LDFLAGS=-lythread
TOOL_LDFLAGS=$(LDFLAGS) -lgflags -lyhttp

all: libbtunnel.a register_service tcp_proxy bproxy bproxy_server buffer_test \
     peer_message_test encoding_test

.cpp.o:
	$(CC) $(CFLAGS) -c $<

libbtunnel.a: tcpserver.o util.o registration.o tunnel.o service.o browse.o \
              buffer.o peer.o encoding.o peer_message.o
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

test: all
	./buffer_test
	./encoding_test
	./peer_message_test

clean:
	rm -f *.o *.a core *_test tcp_proxy register_service bproxy bproxy_server
