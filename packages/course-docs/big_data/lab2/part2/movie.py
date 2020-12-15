# Part 2 starter code

from mrjob.job import MRJob
from mrjob.step import MRStep
import numpy as np
import sys, os

class MRMovielens(MRJob):

    def dist_vec(self,v1,v2):
        #calculate the ditance between two vectors (in two dimensions)
        return np.sqrt((v2[0]-v1[0])*(v2[0]-v1[0])+(v2[1]-v1[1])*(v2[1]-v1[1]))

    def mapper(self, _, value):
        centroids = list()
        for i in os.listdir("centroids"):
            x, y = open("centroids/"+i, "r").readline().strip("\n").split(",")
            centroids.append([float(x), float(y)])
        age, rating = value.split(",")
            age, rating = v.split(",")
            point = [float(age), float(rating)]
            min_dist = sys.maxint
            classe = 0
            for i in range(len(centroids)):
                #ist = self.dist_vec(point, centroids[i])
                if dist < min_dist:
                    #min_dist = dist
                    #classe = i
        yield classe, point

    def combiner(self, key, value):
        count = 0
        moy_x = moy_y=0.0
        for t in value:
            count += 1
            moy_x+=t[0]
            moy_y+=t[1]
        yield key, (moy_x/count,moy_y/count)

    def reducer(self, key, value):
        count = 0
        moy_x = moy_y = 0.0
        for t in value:
            count += 1
            moy_x += t[0]
            moy_y += t[1]
        yield (None, str(key)+","+str(moy_x/count)+","+str(moy_y/count))

    def steps(self):
        # This function defines the steps your job will follow. If you want to chain jobs, you can just have multiple steps.
        step = []
        for i in range(1000):
            step.append(MRStep(mapper=self.mapper, reducer=self.reducer))
        return step

if __name__ == '__main__':
    MRMovielens.run()
