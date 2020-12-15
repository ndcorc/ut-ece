#!/usr/bin/env python
import sys

# Input comes from the standard input line by line (not word by word!)
for line in sys.stdin:
    # TODO: Implement the map part of the word count procedure
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into lowercase words
    words = line.lower().split()
    count = len(words)
    for i in range(0, count-1):
        bigram = (words[i], words[i+1])
        print '%s\\\\%s\t%s' % (bigram[0],bigram[1], 1)

