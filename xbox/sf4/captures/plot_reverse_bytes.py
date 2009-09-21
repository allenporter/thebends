#!/usr/bin/python

"""Plots the change of a byte, over time.  The byte offset is relative to the end of the packet."""

import plot
import sys
import struct

class PacketsPerSecondByHost(plot.Plot):

  def __init__(self, relative_host):
    plot.Plot.__init__(self)
    self._relative_host = relative_host
    self._counter = 0

  def AddPacket(self, ts, packet):
    self._counter = self._counter + 1
    self._ExtendTs(self._counter)
    if "65.55.42.180" in [packet.src_ip(), packet.dst_ip()]:
      # Skip Xbox live server packets, only include game packets
      return
    host_label = None
    if packet.src_ip() != self._relative_host:
      # Only look at outgoing packets
      return
    packet_type = struct.unpack('B', packet.data()[0])[0]
    if packet_type != 0x8d:
      return
    # The first byte seems to indicate what type of packet we see
    for i in xrange(2, 4):
      offset = 0 - i
      data = packet.data()[offset-2:offset]
      value = struct.unpack('H', data)[0]
      label = 'offset=%d' % (offset)
      self._Set(self._counter, label, value)

def main(argv=None):
  if not argv:
    argv = sys.argv
  if len(argv) != 3:
    print >>sys.stderr, "Usage: %s <xbox ip> <pcap filename>" % (argv[0])
    return 2 
  host = argv[1]
  filename = argv[2]
  p = PacketsPerSecondByHost(host)
  plot.PlotPackets(filename, 'reverse', p, "w linespoints")
  return 0

if __name__ == "__main__":
  sys.exit(main())
