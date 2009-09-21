#!/usr/bin/python

"""A plot of packets per second, broken down by source and destination host"""

import plot
import sys

class PacketsPerSecondByHost(plot.Plot):

  def __init__(self, relative_host):
    plot.Plot.__init__(self)
    self._relative_host = relative_host

  def AddPacket(self, ts, packet):
    self._ExtendTs(ts)
    host_label = None
    if packet.src_ip() == self._relative_host:
      host_label = '> %s' % (packet.dst_ip())
    elif packet.dst_ip() == self._relative_host:
      host_label = '< %s' % (packet.src_ip())
    else:
      return
    self._Inc(ts, host_label)

def main(argv=None):
  if not argv:
    argv = sys.argv
  if len(argv) != 3:
    print >>sys.stderr, "Usage: %s <xbox ip> <pcap filename>" % (argv[0])
    return 2 
  host = argv[1]
  filename = argv[2]
  p = PacketsPerSecondByHost(host)
  plot.PlotPackets(filename, 'pps', p, "w linespoints")
  return 0

if __name__ == "__main__":
  sys.exit(main())
