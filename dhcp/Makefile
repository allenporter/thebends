CC = gcc
CCFLAGS = -Wall -g -I/sw/include
LDFLAGS = -L/sw/lib -lnet
AR = ar
RANLIB = ranlib

.SUFFIXES: .c

.c.o:
	$(CC) $(CCFLAGS) -c $<

all: dhcp_client test

test: options_test messages_test

libdhcp.a: options.o messages.o
	$(AR) rc $@ options.o messages.o
	$(RANLIB) $@

dhcp_client: libdhcp.a dhcp_client.o
	$(CC) $(CCFLAGS) -o $@ dhcp_client.o -L./ -ldhcp $(LDFLAGS)

options_test: libdhcp.a options_test.o
	$(CC) $(CCFLAGS) -o $@ options_test.o -L./ -ldhcp $(LDFLAGS)

messages_test: libdhcp.a messages_test.o
	$(CC) $(CCFLAGS) -o $@ messages_test.o -L./ -ldhcp $(LDFLAGS)

clean:
	-rm -fr *.o *.a core dhcp_client *_test
