CC=arm-apple-darwin-gcc
CFLAGS=-Wall -Werror
LDFLAGS=-lobjc -framework CoreFoundation -framework Foundation \
        -framework UIKit -framework TelephonyUI

all:	LCDView

LCDView: main.o LCDView.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o LCDView
