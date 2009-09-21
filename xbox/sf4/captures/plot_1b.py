#!/usr/bin/python

"""Plots the first byte of the UDP packet contents, by src/dst host"""

import plot
import sys
import struct

class PacketsPerSecondByHost(plot.Plot):

  def __init__(self, relative_host):
    plot.Plot.__init__(self)
    self._relative_host = relative_host

  def AddPacket(self, ts, packet):
    self._ExtendTs(ts)
    if "65.55.42.180" in [packet.src_ip(), packet.dst_ip()]:
      # Skip Xbox live server packets, only include game packets
      return
    host_label = None
    if packet.src_ip() == self._relative_host:
      host_label = '> %s' % (packet.dst_ip())
    elif packet.dst_ip() == self._relative_host:
      host_label = '< %s' % (packet.src_ip())
    else:
      return
    # The first byte seems to indicate what type of packet we see
    packet_type = struct.unpack('B', packet.data()[0])[0]
    label = '0x%x %s' % (packet_type, host_label)
    self._Inc(ts, label)

def main(argv=None):
  if not argv:
    argv = sys.argv
  if len(argv) != 3:
    print >>sys.stderr, "Usage: %s <xbox ip> <pcap filename>" % (argv[0])
    return 2 
  host = argv[1]
  filename = argv[2]
  p = PacketsPerSecondByHost(host)
  plot.PlotPackets(filename, '1b', p, "w linespoints")
  return 0

if __name__ == "__main__":
  sys.exit(main())
