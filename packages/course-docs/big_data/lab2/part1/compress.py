#! /usr/bin/python

# Import standard packages
import sys
import os
import random
import numpy as np
# Import Pillow
from PIL import Image, ImageFilter

# Grab arguments
img, k = sys.argv[1], int(sys.argv[2])
# Read image
im = Image.open(img)
# Load all the pixels
px = im.load()

def converged(mean, old):
    return set([tuple(a) for a in mean]) == set([tuple(a) for a in old])

coordinates = np.array([[x, y] for x in range(im.size[0]) for y in range(im.size[1])])
mean = random.sample(coordinates, k)
old = random.sample(coordinates, k)

iteration = 0
while not converged(mean, old):
    if iteration > 100: break
    old = mean
    new = []
    clusters = {}
    for xy in coordinates:
        best_mean = min([(i[0], np.linalg.norm(xy-mean[i[0]])) \
                           for i in enumerate(mean)], key=lambda t:t[1])[0]
        try:
            clusters[best_mean].append(xy)
        except KeyError:
            clusters[best_mean] = [xy]
    keys = sorted(clusters.keys())
    for k in keys:
        new.append(np.mean(clusters[k], axis = 0))
    mean = new
    iteration += 1
mean = np.round(mean)
colors = []
for i in mean:
    colors.append(np.array(px[i[0], i[1]]))
for cluster in clusters:
    for xy in clusters[cluster]:
        point = np.array(px[xy[0], xy[1]])
        new_color, dist = tuple(colors[0]), np.linalg.norm(colors[0]-point)
        for color in colors:
            if(dist > np.linalg.norm(color-point)):
                new_color, dist = tuple(color), np.linalg.norm(color-point)
        px[xy[0], xy[1]] = new_color
im.save("result.png")
im.show()
