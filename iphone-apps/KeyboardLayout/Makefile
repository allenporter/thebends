CC=arm-apple-darwin-gcc
CFLAGS=-Wall -Werror -O7
LDFLAGS=-lobjc -framework CoreFoundation \
        -framework Foundation -framework UIKit -framework CoreGraphics \
        -framework GraphicsServices

all:	KeyboardLayout

KeyboardLayout: main.o KeyboardLayout.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o KeyboardLayout
