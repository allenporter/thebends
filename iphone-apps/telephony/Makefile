CC=arm-apple-darwin-gcc
#CFLAGS=-Wall -Werror
LDFLAGS=-framework CoreFoundation -framework CoreTelephony

all:	telephony

telephony: telephony.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o:	%.m
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o telephony
