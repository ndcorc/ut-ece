import pandas as pd
from sklearn import preprocessing
from sklearn import cross_validation as cv

def get_data():
    df = pd.read_csv("../dataset/train.data", sep=", ", names=[
        "age", "workclass", "fnlwgt", "education", "education-num", "marital-status",
        "occupation", "relationship", "race", "sex", "capital-gain", "capital-loss",
        "hours-per-week", "native-country", "salary"
    ], index_col=False)
    return df

def test_data():
    df = pd.read_csv("../dataset/test.data", sep=", ", names=[
        "age", "workclass", "fnlwgt", "education", "education-num", "marital-status",
        "occupation", "relationship", "race", "sex", "capital-gain", "capital-loss",
        "hours-per-week", "native-country", "salary"
    ], index_col=False)
    return df

def clean_mode(df):
    for feature in df.columns:
        mode = df[feature].mode()
        for index, value in enumerate(df[feature]):
            if value == "?":
                df.set_value(index, feature, mode)
    return df

def clean(df):
    rows = []
    features = [
        "workclass", "education", "marital-status", "occupation",
        "relationship", "race", "sex", "native-country"
    ]
    keys = {}
    for feature in df.columns:
        if feature in features: keys[feature] = {}
        for index, value in enumerate(df[feature]):
            if value == "?" and index not in rows:
                rows.append(index)
            elif feature in features:
                if value not in keys[feature]:
                    keys[feature][value] = index
                    keys[feature][index] = value
                    df.set_value(index, feature, keys[feature][value])
                else:
                    df.set_value(index, feature, keys[feature][value])
    df = df.drop(rows)
    salary = list()
    for index, s in enumerate(df['salary']):
        if s == ">50K" or s == ">50K.": salary.append(1)
        else: salary.append(0)
    del df['salary']
    salary = pd.Series(salary)
    return df, salary, keys



def split_data(df, salary, size, state):
    return cv.train_test_split(df, salary, test_size=size, random_state=state)

def get_features():
    return [
        "age", "workclass", "fnlwgt", "education", "education-num",
        "marital-status", "occupation", "relationship", "race", "sex",
        "capital-gain", "capital-loss", "hours-per-week", "native-country"
    ]
