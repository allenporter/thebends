CC=arm-apple-darwin-cc
CPPFLAGS=-Wall -Werror
LD=$(CC)
LDFLAGS=-lobjc -framework CoreFoundation -framework Foundation \
        -framework UIKit -framework LayerKit -framework CoreGraphics

all:	TextDrawing

TextDrawing: main.o TextDrawing.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o TextDrawing
