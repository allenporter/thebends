CC=arm-apple-darwin-cc
CPPFLAGS=-Wall
LD=$(CC)
LDFLAGS=-lobjc -framework CoreFoundation -framework Foundation -framework UIKit -framework LayerKit

all:	TextViewDemo

TextViewDemo:	main.o TextViewDemo.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o TextViewDemo
