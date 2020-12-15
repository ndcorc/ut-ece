#!/usr/bin/env python
import sys
from operator import itemgetter

# same as the mapper, input comes from the standard input line by line
# TODO: Implement the reduce part of the word count procedure
word_count = {}
for line in sys.stdin:
    line = line.strip()
    word, count = line.split('\t', 1)
    try:
        count = int(count)
        word_count[word] = word_count.get(word, 0) + count
    except ValueError:
        pass

sorted_word_count = sorted(word_count.items(), key=itemgetter(0))
for word, count in sorted_word_count:
    print '%s\t%s'% (word, count)

