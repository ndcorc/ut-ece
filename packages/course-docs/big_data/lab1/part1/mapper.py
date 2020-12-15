#!/usr/bin/env python
import sys

def resolve(word):
    word1, word2 = word.split("'")
    lastletter = word1[len(word1)-1]
    if lastletter.lower() == 'n':
        word1 = word1.replace(lastletter, '')
        word2 = lastletter + 'o' + word2
        if word1.lower() == 'ca':
            word1 += 'n'
        elif word.lower() in ["i'm", "you're"]:
            word2 = 'a' + word2
        else:
            word2 = 'ha' + word2
    return word1, word2

# Input comes from the standard input line by line (not word by word!)
for line in sys.stdin:
    # TODO: Implement the map part of the word count procedure
    # remove leading and trailing whitespace
    line = line.strip()
    # split the line into words
    words = line.split()
    for word in words:
        if word.lower() in ["i'm", "i've", "aren't", "can't", "didn't", "couldn't",
                            "shouldn't", "isn't", "you're"]:
            word1, word2 = resolve(word)
            print '%s\t%s\n%s\t%s' % (word1, 1, word2, 1)
        else:
            print '%s\t%s' % (word, 1)

