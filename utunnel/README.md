# utunnel - A UDP Tunnel

2004 - Allen Porter <allen@thebends.org>

# Intro

utunnel allows you to tunnel ip traffic (tcp, icmp, etc) over udp.
utunnel is written and c using libpcap and libnet.  utunnel is very
much unfinished.

Note: It also allows for the endpoints of these packets to be spoofed
as other machines on your unswitched LAN (think wireless or hubs).  This
tool is simply a proof of concept and the actual uses are left to your
imagination. 

# Requirements

utunnel has been tested on mac os x and freebsd.  Please let me know
if you are successful in getting this to run on your os and what
changes were necessary. utunnel was developed using the following
libraries:

* libpcap >= 0.8.3
* libnet >= 1.1.2.1

# Build

Install instructions can be found in the INSTALL file, but the eager can
get away with issuing the following commands:

```
./configure
make
make install
```

# Usage

Invoke uclient or userver with no arguments for a quick and
dirty usage explanatioin.

# TODO

I had planned these additions to utunnel, but don't have any intentions
to work on them anytime soon:

* usage documentation
* re-implement support for mac spoofing
* mtu sized packets are not handled properly
* experiment with tunneling over other protocols such as gre
* add support for linux loopback oddities
* test with various NATs
* write custom session handlers for all supported protocols

# Acknowledgement

Extremely helpful implementation advice provided by:

* [David Hulton](https://www.linkedin.com/in/h1kari)
* [Dan Kaminsky](http://www.doxpara.com)
