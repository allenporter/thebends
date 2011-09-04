#!/usr/bin/python
#
# Opens the dictionary file containing syllable counts and runs the different
# counting functions for each word to score them.

import re
import sys
import adhoc
import hyphenate
import random

def sample_wr(population, k):
  "Chooses k random elements (with replacement) from a population"
  n = len(population)
  _random, _int = random.random, int  # speed hack 
  result = [None] * k
  for i in xrange(k):
      j = _int(_random() * n)
      result[i] = population[j]
  return result

def hyphenate_word(word):
  """Adapter around the hyphenate function"""
  return len(hyphenate.hyphenate_word(word))

def average(word):
  c = adhoc.syllable_count(word)
  d = hyphenate_word(word)
  if c == d:
    return c
  else:
    return (c + d) / 2

def maximum(word):
  c = adhoc.syllable_count(word)
  d = hyphenate_word(word)
  if c == d:
    return c
  else:
    return max(c, d)

# The functions to test.  The input is a word and the output is a count
COUNTING_FUNCTIONS = {
    'Ad hoc #1': adhoc.syllable_count,
#    'Ad hoc #2': adhoc.syllable_count2,
    'Hyphenation': hyphenate_word,
    'Average': average,
    'Max': maximum,
  }

def parse_line(line):
  """Reads the line from the dictionary, and gets the word and syllable count"""
  word = ''
  count = 0 
  groups = re.split('[^A-Za-z]+', line)
  for match in groups:
    if match:
      word = word + match
      count = count + 1
  return [word, count]

def main(args):
  print 'Reading dictionary'
  lines = [line.strip() for line in open('gcide.out').readlines()]
  print 'Scoring'
  skipped = 0
  success = {}
  failures = {}
  for line in lines:
    if line.find(' ') != -1:
      skipped = skipped + 1
      continue
    [word, count] = parse_line(line)
    if count == 1:
      skipped = skipped + 1
      continue
    any_success = False
    for (name, func) in COUNTING_FUNCTIONS.iteritems():
      result = func(word)
      if result == count:
        any_success = True
        success[name] = success.get(name, 0) + 1
      else:
        d = failures.get(name, {})
        d.update({word: result})
        failures[name] = d
    if any_success:
      success['Any'] = success.get('Any', 0) + 1
    else:
      d = failures.get('Any', {})
      d.update({word: count})
      failures['Any'] = d

  total = len(lines) - skipped
  print
  print 'Total words: %d' % total
  for name in COUNTING_FUNCTIONS.keys() + ['Any']:
    count = success[name]
    percent = 100.0 * count / total
    print '%-15s: %2.2f (%d)' % (name, percent, count)

  sample_words = sample_wr(failures['Any'].keys(), 10)
  print
  print 'Failures'
  for word in sample_words:
    count = failures['Any'][word]
    print '%s %d:' % (word, count)
    for name in COUNTING_FUNCTIONS.keys():
      wrong_score = failures[name][word]
      print '  %s: %d' % (name, wrong_score)

if __name__ == "__main__":
  sys.exit(main(sys.argv))
