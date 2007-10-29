CC=arm-apple-darwin-gcc
CFLAGS=-Wall -Werror
LD=$(CC)
LDFLAGS=-lobjc -framework CoreFoundation -framework Foundation \
        -framework UIKit -framework CoreGraphics

all:	TileView

TileView: main.o TileView.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o TileView
