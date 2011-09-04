#!/usr/bin/python
#
# 

import re 

VOWELS = 'aeiouy'

def is_vowel(c):
  return VOWELS.find(c) != -1

def syllable_count(word):
  """Uses an ad-hoc approach for counting syllables in a word"""
  # Count the vowels in the word
  # Subtract one vowel from every dipthong
  count = len(re.findall(r'([aeiouyAEIOUY]+)', word))
  # Subtract any silent vowels
  if len(word) > 2:
    if word[-1] == 'e' and  \
       not is_vowel(word[-2]) and \
       is_vowel(word[-3]):
      count = count - 1
  return count
