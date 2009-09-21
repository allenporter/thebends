"""A library that read pcap files and return the enclosed packets."""

import struct

# See the following wiki page for details on the libpcap file format:
# http://wiki.wireshark.org/Development/LibpcapFileFormat
GLOBAL_HEADER_FORMAT = 'IhhiIII'
PACKET_HEADER_FORMAT = 'IIII'

class PcapFileReader(object):
  """A class that reads pcap data files and returns the underlying packets"""

  def __init__(self, f):
    """Creates a pcap file reader for the specified file object."""

    self._f = f
    raw_header = f.read(struct.calcsize(GLOBAL_HEADER_FORMAT))
    # Unpack some of the header contents.  Currently we drop them, but this at least provides a sanity check
    # that the contents of the file are un-packable.
    header = struct.unpack(GLOBAL_HEADER_FORMAT, raw_header)
    self._magic = header[0]
    self._version = "%d.%d" % (header[1], header[2])
    self._snaplen = header[5]

  def __iter__(self):
    return self

  def next(self):
    """Returns a tuple of timestamp and packet."""
    raw_header = self._f.read(struct.calcsize(PACKET_HEADER_FORMAT))
    if not raw_header:
      raise StopIteration
    header = struct.unpack(PACKET_HEADER_FORMAT, raw_header)
    packet = self._f.read(header[2])
    ts = header[0]
    return [ts, packet]
