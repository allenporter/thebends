all: client server

client: client.c
	/usr/local/arm-apple-darwin/bin/gcc -fsigned-char -Wall -Werror \
           -Wl,-syslibroot,$(HEAVENLY) \
           -o client client.c

server: server.c
	gcc -Wall -Werror -o server server.c

clean:
	rm -f *.o client server
