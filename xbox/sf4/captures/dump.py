#!/usr/bin/python

import struct
import sys
import udp

"""Dumps packets and attempts to annotate them."""

class Printer:
  def __init__(self, relative_host):
    self._relative_host = relative_host
    self._relative_id = None
    self._remote_id = None

  def PrintPacket(self, ts, packet):
    host_label = None
    if '65.55.42.180' in [ packet.src_ip(), packet.dst_ip() ]:
      return
    data = packet.data()
    packet_type = data[0:1].encode('hex')
    host_id = data[1:4].encode('hex')
    clock = struct.unpack('H', data[-12:-10])[0]
    tail = data[-10:]
    data = data[5:-12]
    if packet.src_ip() == self._relative_host:
      if not self._relative_id:
        self._relative_id = host_id
      elif host_id != self._relative_id:
        raise Error('id mismatch (%s != %s)' % (host_id, self._relative_id))
      host_label = '%s >' % host_id
    elif packet.dst_ip() == self._relative_host:
      return
      if not self._remote_id:
        self._remote_id = host_id
      elif host_id != self._remote_id:
        raise Error('id mismatch (%s != %s)' % (host_id, self._remote_id))
      host_label = '%s <' % host_id
    else:
      return
    print '%s:%04d %s [%s][%s][%s]' % (ts, clock, host_label, packet_type, 
                                    data.encode('hex'), tail.encode('hex'))

def main(argv=None):
  if not argv:
    argv = sys.argv
  if len(argv) != 3:
    print >>sys.stderr, "Usage: %s <xbox ip> <pcap filename>" % (argv[0])
    return 2 
  host = argv[1]
  filename = argv[2]
  printer = Printer(host)
  reader = udp.UdpPacketReader(filename)
  for (ts, p) in reader:
    printer.PrintPacket(ts, p)

  return 0

if __name__ == "__main__":
  sys.exit(main())

