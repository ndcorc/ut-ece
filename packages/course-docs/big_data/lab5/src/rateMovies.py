#!/usr/bin/env python

import sys
from os import remove, removedirs
from os.path import dirname, join, isfile
from time import time
import pandas as pd
import numpy as np

def rate():
    movies = pd.read_csv("../data/ml-100k/u.item", sep="|", quoting=2, names=[
            "movie id", "movie title", "release date", "video release date", "IMDb URL",
            "unknown", "Action", "Adventure", "Animation", "Children's", "Comedy",
            "Crime", "Documentary", "Drama", "Fantasy", "Film-Noir", "Horror",
            "Musical", "Mystery", "Romance", "Sci-Fi", "Thriller", "War", "Western"
    ])
    movies = movies.as_matrix(columns=['movie id','movie title'])
    tenRandom = list()
    for i, v in enumerate(np.random.choice(len(movies), 10)):
        tenRandom.append(list(movies[v]))

    parentDir = dirname(dirname(__file__))
    ratingsFile = join(parentDir, "personalRatings.txt")

    if isfile(ratingsFile):
        r = raw_input("Looks like you've already rated the movies. Overwrite ratings (y/N)? ")
        if r and r[0].lower() == "y":
            remove(ratingsFile)
        else:
            sys.exit()

    prompt = "Please rate the following movie (1-5 (best), or 0 if not seen): "
    print prompt

    now = int(time())
    n = 0

    f = open(ratingsFile, 'w')
    for movie in tenRandom:
        valid = False
        while not valid:
            rStr = raw_input(movie[1] + ": ")
            r = int(rStr) if rStr.isdigit() else -1
            if r < 0 or r > 5:
                print prompt
            else:
                valid = True
                if r > 0:
                    f.write("0::%s::%d::%d\n" % (movie[0], r, now))
                    n += 1
    f.close()

    if n == 0:
        print "No rating provided!"
