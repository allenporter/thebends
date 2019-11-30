# thebends

*Misc projects from thebends.org*

## Projects

See https://github.com/allenporter for an overview of all my projects.  This
projects *thebends* contains smaller projects that were created before
github existed -- though arguably could each be their own github project.

* [avr](avr/) -  I like playing with Atmel AVR microcontrollers, especially when mixing them with networks.
* [libnatpmp](nat/pmp/) - NAT-PMP is a protocol for routers to allow internal clients to create port forwarding entries dynamically.  I wrote a library for speaking nat-pmp, and a few tools to go along with it.  Someone else wrote a [library](http://miniupnp.free.fr/libnatpmp.html) called libnatpmp after I did, but isn't interested in changing the name.  They also plastered it all over the web.  Too bad for me, I guess.
* [utunnel](utunnel/) -  Written using libnet and pcap, utunnel allows you to tunnel IP traffic over udp.  Only tested on BSD based unixes, it may be a little rough around the edges, so use at your own risk.
* [btunnel](btunnel/) - A work in progress, btunnel (bonjour tunnel) is a tool for tunneling bonjour packets between networks.  While bonjour and mDNS are usually LAN based, Its not clear if [other methods](http://www.dns-sd.org/) that I haven't read enough about yet might work better, using existing protocols.  I think I would rather expose my zero configuration services to my trusted friends only.
* [ythread](ythread/) -   ythread is a simple C++ thread/mutex library that wraps the pthread library.  I wanted a simple way to create threads from my C++ applications and thought maybe someone might find this simple set of classes useful.
* [yhttp](yhttp/) - HTTP is everywhere, so why not link an HTTP server into all of your long running processes?  yhttp is a very basic HTTP server library.  I like the idea of ditching the bloated standalone HTTP server + server side code in favor of a small binaries that do one thing, and do it well.  You probably already always have a web browser open, so exposing status of your long running applications over HTTP seems more useful than stdout.  (I'm not claiming credit for coming up with this idea, but I think its something I would like to see more of).    This library is the result of a weekend of work and is nowhere near complete (it doesn't support all of the protocol, for example), but I was able to get a hello world out of it.
* [jspng](jspng/) - jspng is a library to create PNG images written entirely in javascript, mostly as a proof of concept.  I'm not aware of any other similar libraries, which is a shame (though I understand, because it seems a bit slow to make anything that looks good).  To create an image, you just need to give it an array of pixels.  It renders the image with src="data:image/png;base64,...", where the base64 encoded png is created on the fly.  See the unit tests for implementation details.  I would love to hear from any javascript experts who can speed it up.  Also, let me know if you use the code for anything useful, or if you end up doing the work to make it support color, etc.  Mostly, i'm curious if anyone else found the RFC as hard to understand as I did!
* [MotionScreen](MotionScreen/) - MotionScreen is an OS X application that uses an iSight camera to transpose foreground images on some background.  You first select a background image, then a take a reference snapshot and jump in front of the camera.  Using motion or differences from a scene seemed like a good way to make a quick and dirty "green screen".  
* [ramdisk](ramdisk/) - Just for fun, a MacFUSE based ramdisk.
* [memedit](memedit/) - A tool for manipulating memory of a running process.  Uses the mach API and zone introspection to iterate over regions of heap memory allocated to a running task, as well as a mode for reading and writing these regions (using mach_vm_read/mach_vm_write). 
* [yprofile](yprofile/) - A simple memory profile, currently supporting the ability to report memory leaks.  This library overrides  malloc and free to track memory allocation and reports memory leaks on shutdown.
* [libypeer](ypeer/) -  Why not use public bittorrent trackers for finding peers for things other than filesharing?  They seem pretty reliable and have a nice peer discovery mechanism (a shared secret).  The libypeer library is written in C++ and does just that.

## Related Projects

* [machsniff](https://github.com/allenporter/machsniff) - A tool for sniffing Mach RPCs
* [MobileSynth](https://github.com/allenporter/mobilesynth) - Synthesizer, for iPhone
* [open-tracker](https://github.com/allenporter/open-tracker) - I needed a server to test out libypeer, so open-tracker was born.  This tracker was built on Google's App Engine infrastructure, written in python.
* [ArpSpyX](https://github.com/allenporter/arpspyx) - ArpSpyX is an arp packet sniffer for OS X that I found useful for surveying my little home network.  It was also an excuse to dabble in Objective-C / Cocoa.  Also see [arpdropper](https://github.com/allenporter/thebends/blob/master/misc/arpdropper.c) for injecting arbitrary arp request and arp reply packets.
* [MobileTerminal](https://github.com/allenporter/mobileterminal) - A terminal emulator application for the iPhone.
