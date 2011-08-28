#!/usr/bin/python
#
# Extracts words by syllables from the gcide dictionary, obtained from
# http://ftp.gnu.org/old-gnu/gcide/

import sys
import os
import re

def filter_sylables(lines):
  pat = re.compile("<p><hw>([A-Z].*?)</hw>")
  for line in lines:
    w = pat.match(line)
    if not w:
      continue
    word = w.group(1)
    if word.find("\\'") > -1:
      # Skip any words that require unescaping -- they aren't interesting for
      # our purposes.
      continue
    print word

def main(args):
  if len(args) != 2:
    print "Usage: %s <gcide path>" % args[0]
    return

  gcide_path = args[1]
  files = os.listdir(gcide_path)
  for name in files:
    if not name.startswith("CIDE"):
      continue
    lines = open("/".join([ gcide_path, name ])).readlines()
    filter_sylables(lines)

if __name__ == "__main__":
  sys.exit(main(sys.argv))
