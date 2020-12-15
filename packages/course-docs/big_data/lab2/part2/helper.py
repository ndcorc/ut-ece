import pandas as pd
import numpy as np

data = pd.read_csv("ml-100k/u.data", sep="\t", quoting=2, names=[
    "user id", "item id", "rating", "timestamp"
])
users = pd.read_csv("ml-100k/u.user", sep="|", quoting=2, names=[
    "user id", "age", "gender", "occupation", "zip code"
])
items = pd.read_csv("ml-100k/u.item", sep="|", quoting=2, names=[
    "movie id", "movie title", "release date", "video release date", "IMDb URL",
    "unknown", "Action", "Adventure", "Animation", "Children's", "Comedy",
    "Crime", "Documentary", "Drama", "Fantasy", "Film-Noir", "Horror",
    "Musical", "Mystery", "Romance", "Sci-Fi", "Thriller", "War", "Western"
])

def get_coordinates(users, items, data):
    f = open("coordinates.txt", "w")
    for user, age in zip(users['user id'], users['age']):
        ratings = list()
        user_data = data.loc[data['user id'] == user]
        for item, rating in zip(user_data['item id'], user_data['rating']):
            movie = items.loc[items['movie id'] == item]
            if movie['Documentary'].values[0] == 1:
                ratings.append(rating)
        if len(ratings) > 0:
            avg_rating = str(np.mean(ratings))
            age = str(age)
            f.write("%s\t%s\n" % (age, avg_rating))

get_coordinates(users, items, data)
