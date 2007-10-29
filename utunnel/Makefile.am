bin_PROGRAMS = userver uclient

userver_SOURCES = utunnel-server.c utunnel.c utunnel.h \
		    packet-functions.c packet-functions.h \
		    translate.c translate.h
userver_LDADD = -lpcap -lnet

uclient_SOURCES = utunnel-client.c utunnel.c utunnel.h \
		    packet-functions.c packet-functions.h \
		    translate.c translate.h
uclient_LDADD = -lpcap -lnet
