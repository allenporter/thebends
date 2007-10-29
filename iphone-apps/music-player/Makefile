CC=arm-apple-darwin-gcc
CFLAGS=-Wall -Werror
LDFLAGS=-framework CoreFoundation -framework MobileMusicPlayer

all:	song-info

%: %.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o song-info
