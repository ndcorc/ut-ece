#!/usr/bin/env python
import sys
from operator import itemgetter

# same as the mapper, input comes from the standard input line by line

bigrams = {}
b1 = {}
for line in sys.stdin:
    line = line.strip()
    bigram, count = line.split('\t', 1)
    bigram = tuple(bigram.split('\\\\'))
    try:
        count = int(count)
        bigrams[bigram] = bigrams.get(bigram, 0) + count
        b1[bigram[0]] = b1.get(bigram[0], 0) + count
    except ValueError:
        pass
# unique bigram count - bigram_count.txt
#print '%s\t%s' % ('Unique Bigram Count:', len(bigrams))

# top ten bigrams - bigram_topten.txt
#sorted_bigrams = sorted(bigrams.items(), key=itemgetter(1), reverse=True)
#print 'Top 10 Bigrams:'
#for i in range(0, 10):
#    print '%s: %s\t=\t %s' % (i+1, sorted_bigrams[i][0], sorted_bigrams[i][1])

# relative frequencies - bigram_relative.txt
for key in bigrams:
    prob = 100*float(bigrams[key])/float(b1[key[0]])
    print 'P(%s|%s) = %s%s' % (key[1], key[0], prob, '%')
