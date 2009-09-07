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

class Usage(Exception):
  def __init__(self, msg):
    self.msg = msg

def parse_arguments(opts):
  host = None
  filename = None
  for o, a in opts:
    if o in ("-h", "--help"):
      print __doc__
      sys.exit(0)
    if o == "--host":
      host = a
    if o == "--file":
      filename = a
  if host is None or filename is None:
    print __doc__
    sys.exit(0)
  return [ host, filename ]

# See pcap file format at:
# http://wiki.wireshark.org/Development/LibpcapFileFormat
GLOBAL_HEADER_FORMAT = 'IhhiIII'
PACKET_HEADER_FORMAT = 'IIII'

class PcapReader(object):
  """Reads pcap files"""

  def __init__(self, f):
    self._f = f
    raw_header = f.read(struct.calcsize(GLOBAL_HEADER_FORMAT))
    header = struct.unpack(GLOBAL_HEADER_FORMAT, raw_header)
    self._magic = header[0]
    self._version = "%d.%d" % (header[1], header[2])
    self._snaplen = header[5]
    #print "Pcap: %s snaplen: %d" % (self._version, self._snaplen)

  def __iter__(self):
    return self

  def next(self):
    raw_header = self._f.read(struct.calcsize(PACKET_HEADER_FORMAT))
    if not raw_header:
      raise StopIteration
    header = struct.unpack(PACKET_HEADER_FORMAT, raw_header)
    packet = self._f.read(header[3])
    return [header[0], packet]

def IpToString(ip):
  return "%d.%d.%d.%d" % ((ip & 0xff000000) >> 24,
                          (ip & 0xff0000) >> 16,
                          (ip & 0xff00) >> 8,
                          ip & 0xff)

# Assumes that TCP options are not present
MAC_HEADER_FORMAT = "!6B6B2B"
MAC_HEADER_SIZE = struct.calcsize(MAC_HEADER_FORMAT)
IP_HEADER_FORMAT = "!BBhhHBBHII"
IP_HEADER_SIZE = struct.calcsize(IP_HEADER_FORMAT)
UDP_HEADER_FORMAT = "!hhhH"
UDP_HEADER_SIZE = struct.calcsize(UDP_HEADER_FORMAT)

class PacketException(Exception):
  def __init__(self, m):
    self._m = m

  def __str__(self):
    return self._m

class UdpPacket(object):
  def __init__(self, packet):
    self._packet = packet
    raw_mac_header = packet[0:MAC_HEADER_SIZE]

    packet = packet[MAC_HEADER_SIZE:]
    raw_ip_header = packet[0:IP_HEADER_SIZE]

    packet = packet[IP_HEADER_SIZE:]
    raw_udp_header = packet[0:UDP_HEADER_SIZE]

    self._data = packet[UDP_HEADER_SIZE:]

    ip_header = struct.unpack(IP_HEADER_FORMAT,  raw_ip_header)
    (v, tos, data_len, id, flags, ttl, proto, checksum, src_ip, dst_ip) = \
        ip_header
    version = (ip_header[0] & 0xf0) >> 4
    if version != 4:
      raise PacketException("Unexpected ip version: %d" % version)
    if (ip_header[0] & 0x0f) != 5:
      raise PacketException("Can't handle packet with options")
    if proto != 17:  # UDP
      raise PacketException("Got packet with unexpected proto: %d" % proto)
    self._src_ip = IpToString(src_ip)
    self._dst_ip = IpToString(dst_ip)

    udp_header = struct.unpack(UDP_HEADER_FORMAT, raw_udp_header)
    (src_port, dst_port, udp_len, checksum)  = udp_header
    found_len = UDP_HEADER_SIZE + len(self._data)
    if udp_len != found_len:
      raise PacketException("Udp header expects: %d, got %d" % (udp_len, found_len))
    self._src_port = src_port
    self._dst_port = dst_port

  def src_ip(self):
    return self._src_ip

  def src_port(self):
    return self._src_port

  def dst_ip(self):
    return self._dst_ip

  def dst_port(self):
    return self._dst_port

  def data(self):
    return self._data

def main(argv=None):
  if argv is None:
    argv = sys.argv
  try:
    try:
      opts, args = getopt.getopt(argv[1:], "h", ["help", "host=", "file="])
    except getopt.error, msg:
      raise Usage(msg)
    # more code, unchanged
  except Usage, err:
    print >>sys.stderr, err.msg
    print >>sys.stderr, "for help use --help"
    return 2

  [ host, filename ] = parse_arguments(opts)

  f = open(filename, 'rb')
  reader = PcapReader(f)

  min_ts = None
  max_ts = None 
  labels = set()
  data = {}
  for (ts, packet) in reader:
    try:
      p = UdpPacket(packet)
    except PacketException, e:
      continue
    if not min_ts:
      min_ts = ts
    max_ts = ts

    if p.src_ip() == host:
      label = ">%s" % p.dst_ip()
    elif p.dst_ip() == host:
      label = "<%s" % p.src_ip()
    else:
      # Packet not to or from our host
      continue
    labels.add(label)
    index = "%d-%s" % (ts, label)
    count = data.get(index, 0)
    data[index] = count + 1

  print "# ",
  for label in labels:
    print "%s\t" % (label),
  print

  for ts in xrange(min_ts, max_ts):
    for label in labels:
      index = "%d-%s" % (ts, label)
      count = data.get(index, 0)
      print "%d\t" % (count),
    print

#  for (ts, p) in packets:
#    print "%d: %s:%d -> %s:%d: %s" % (ts, p.src_ip(), p.src_port(), p.dst_ip(),
#                                      p.dst_port(), p.data().encode("hex"))

  f.close()


if __name__ == "__main__":
  sys.exit(main())
