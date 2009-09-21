#!/usr/bin/python

"""A UDP packet parser."""

import struct
import pcap

# Assumes that TCP options are not present
MAC_HEADER_FORMAT = "!6B6B2B"
MAC_HEADER_SIZE = struct.calcsize(MAC_HEADER_FORMAT)
IP_HEADER_FORMAT = "!BBhhHBBHII"
IP_HEADER_SIZE = struct.calcsize(IP_HEADER_FORMAT)
UDP_HEADER_FORMAT = "!hhhH"
UDP_HEADER_SIZE = struct.calcsize(UDP_HEADER_FORMAT)

def IpToString(ip):
  return "%d.%d.%d.%d" % ((ip & 0xff000000) >> 24,
                          (ip & 0xff0000) >> 16,
                          (ip & 0xff00) >> 8,
                          ip & 0xff)

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

class UdpPacketReader(object):
  def __init__(self, filename):
    self._f = open(filename, 'rb')
    self._reader = pcap.PcapFileReader(self._f)

  def close(self):
    self._f.close()

  def __iter__(self):
    return self

  def next(self):
    while True:
      [ts, packet] = self._reader.next()
      try:
        p = UdpPacket(packet)
      except PacketException:
        continue
      return [ts, p]
