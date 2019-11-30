# AVR

## Backgroud

This project is for learning about [AVR](http:////www.atmel.com/products/AVR/)
devices, with a particular focus on OS X based development, since finding OS X
specific resources can sometimes be difficult.  It helps to have a decent unix
and C background to fill in some of the gaps in documentation.

## Hardware

Hardware used for this project (in 2007):

* STK500 with ATmega8515
* EXTECH DC Power supply
* Keyspan USB high speed to Serial adapter
* Easy Ethernet AVR

### ATSTK500

The ATSTK500 is a great starter kit.  If you are running windows, it is pretty
easy to get started using AVR Studio.  Under OS X, avr-gcc, avr-libc, and
avrdude are helpful.  For avrdude, I had to use the "stk500v2" programmer type
since my STK500 has firmware version 2.

Overall this is a great board because it comes with LEDs and Switches built in
which are very helpful for writing simple programs or debugging.

### Easy Ethernet AVR
<p>
The Easy Ethernet AVR is a board that combines a Realtek RTL8019AS ethernet
controller and an ATmega32.  Also comes with onboard serial port and AVR ISP
programming port.  I don't have a standalone ISP programmer, but it can be
connected to the 10 pin ISP slot on the STK500.

### Keyspan USB High Speed Serial Adapter<

The above boards come with serial programming interfaces, so in order to
program them from a mac I use USA-19HS usb to serial adapter.

## Software

The essential software needed is avr-gcc, [avr-libc](http://www.nongnu.org/avr-libc),
and [avrdude](http://www.bsdhome.com/avrdude/).  You may also find it useful to
have [uisp](https://www.nongnu.org/uisp/) if you need to set any fuses.  uisp
currently doesn't support the STK500 version 2 firmware, so when I need to
change fuses, I unfortunately need to boot to windows and run AVR Studio.

The avr-libc documentation has good step-by-step instructions for installing
everything you need to do avr development on your mac.
See [Installing the GNU Tool Chain](http://www.nongnu.org/avr-libc/user-manual/install_tools.html).
Another (easier?) option is to use [fink](http://www.finkproject.org/), but you
must configure fink to use the [unstable](http://www.finkproject.org/faq/usage-fink.php?phpLang=en#unstable)
package tree.

## Community

* [AVR Freaks](http://www.avrfreaks.net/)
* [SparkFun Projects](https://www.sparkfun.com/index?p=tutorials)

## Code

Feel free to do whatever you want with this code, though let me know if you
have suggestions.

### count/ - Hello World

This very basic example simply counts from 0 to 255, outputting the value on
port C of the AVR.  When using an STK 500, you can hook up the PORTC pins to
the LED pins and have a nice little show.

### logging/ - Serial Port Logging Library

I found this logging library useful when developing and debugging other AVR
applications.  It support dumping single values as hex or entire buffers as hex
bytes, or NULL terminated strings.

### rtl8019as/ - Realtek RTL8019AS Driver

The Easy Ethernet/AVR comes with a pretty good example driver written by Fred Eady (of <a href="http://www.edtp.com">EDTP Electornics</a>).
I've written a stripped down library based on his examples that will invoke
your callback when a packet is received.

### packet_dump/ - Packet Sniffing

packet_dump is a small program that combines the two libraries above, and logs incoming packets over serial.

Here is some example output of packet_dump. NOTE: The mac addresses have been changed to proect the innocent.

```
[*] packet_dump initialized; waiting for packet
[*] packet received
[ ff ff ff ff ff ff 01 0c a5 ce 86 d6 08 00 45 00 00 54 17 f4 00 00 40 01 
: 4b 52 0a 00 01 65 0a 00 01 ff 08 00 79 a2 2a fa 00 00 46 87 14 30 00 04 
: 0d a5 08 09 0a 0b 0c 0d 0e 0f 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 
: 1e 1f 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35 
: 36 37 85 b0 ff e8 ]
[*] end of packet
[*] packet received
[ ff ff ff ff ff ff 00 13 c3 cd a7 9a 08 06 00 01 08 00 06 04 00 01 00 19 
: e3 dc ab 9d 0a 00 01 04 00 00 00 00 00 00 0a 00 01 65 00 00 00 00 00 00 
: 00 00 00 00 00 00 00 00 00 00 00 00 92 0f 67 6b ]
[*] end of packet
[*] packet received
[ ff ff ff ff ff ff 01 0c a5 ce 86 d6 08 06 00 01 08 00 06 04 00 01 00 0d 
: 93 82 18 1d 0a 00 01 02 00 00 00 00 00 00 0a 00 01 65 00 00 00 00 00 00 
: 00 00 00 00 00 00 00 00 00 00 00 00 96 92 6c 8f ]
[*] end of packet
```



