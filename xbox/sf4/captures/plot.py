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
import udp

class Plot(object):
  def __init__(self):
    self._min_ts = None
    self._max_ts = None
    self._labels = set()
    self._data = {}

  def _ExtendTs(self, ts):
    if not self._min_ts:
      self._min_ts = ts
    self._max_ts = ts

  def _GetTs(self):
    return xrange(self._min_ts, self._max_ts)

  def _Get(self, ts, label):
    index = '%d-%s' % (ts, label)
    return self._data.get(index, 0)

  def _Set(self, ts, label, value):
    self._labels.add(label)
    index = '%d-%s' % (ts, label)
    self._data[index] = value

  def _Inc(self, ts, label):
    self._Set(ts, label, self._Get(ts, label) + 1)

  def PlotCmds(self, filename):
    index = 1
    plots = []
    for label in self._labels:
      plots.append("'%s' using %d title '%s'" % (filename, index, label))
      index = index + 1
    return plots

  def PlotData(self):
    lines = []
    lines.append("# %s" % ("\t".join(self._labels)))
    for ts in self._GetTs():
      cols = []
      for label in self._labels:
        cols.append("%d" % self._Get(ts, label))
      lines.append("\t".join(cols))
    return "\n".join(lines)

def PlotPackets(filename, plotname, plot, opts=""):
  reader = udp.UdpPacketReader(filename)
  for (ts, p) in reader:
    plot.AddPacket(ts, p)

  output_data_filename = '%s-%s.dat' % (filename, plotname)
  output_plt_filename = '%s-%s.plt' % (filename, plotname)
  plot_cmds = plot.PlotCmds(output_data_filename)
  plot_data = plot.PlotData()
  join_str = " %s, " % opts

  f = open(output_plt_filename, 'w')
  f.write('plot %s %s\n' % (join_str.join(plot_cmds), opts))
  f.close()
  
  f = open(output_data_filename, 'w')
  f.write(plot_data)
  f.close()
