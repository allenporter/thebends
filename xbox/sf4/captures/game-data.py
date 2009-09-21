#!/usr/bin/python
#
# Doc

"""Convert tcpdump packet captures to a plot of packets per second to a host.

Usage:
  --host <ip>
  --file <filename>
"""

import struct
import sys
import getopt
import pcap
import udp

def main(argv=None):
  if not argv:
    argv = sys.argv
  if len(argv) != 3:
    print >>sys.stderr, "Usage: %s <filename> <xbox ip>" % (argv[0])
    return 2 

  host = argv[1]
  filename = argv[2]
  reader = UdpPacketReader(filename)
  for (ts, p) in reader:
    if p.src_ip() == "65.55.42.180" or p.dst_ip() == "65.55.42.180":
      continue
    data = p.data()
    # TODO(allen): Look at other types
#    if not data[0] in [ '\x8d', '\x4d', '\xcd' ]:
#      continue
    contents = data[4:-12]
    game_counter = struct.unpack('H', data[-12:-10])[0]
    trailer = data[-10:]
    print "%d: %s:%d -> %s:%d: (%x, %s) [%s][%s]" % (
        ts, p.src_ip(), p.src_port(), p.dst_ip(), p.dst_port(),
        game_counter, trailer.encode("hex"),
        data[0].encode('hex'),
        contents.encode("hex"))


  f.close()


if __name__ == "__main__":
  sys.exit(main())
