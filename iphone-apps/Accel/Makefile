CC=/usr/local/arm-apple-darwin/bin/gcc
CFLAGS=-fsigned-char -Wall -Werror -O7
LDFLAGS=-Wl,-syslibroot,$(HEAVENLY) -lobjc -framework CoreFoundation \
        -framework Foundation -framework UIKit -framework CoreGraphics \
        -framework GraphicsServices

all:	Accel

Accel: main.o Accel.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o Accel
