CC=arm-apple-darwin-gcc
CFLAGS=-Wall -Werror -O7
LDFLAGS=-lobjc -framework CoreFoundation \
        -framework Foundation -framework UIKit -framework LayerKit \
        -framework CoreGraphics -framework OpenGLES \
        -framework CoreSurface

all:	OpenGLDemo

OpenGLDemo: main.o OpenGLDemo.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o OpenGLDemo
