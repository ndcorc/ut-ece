import SVM as CLF
from sklearn import preprocessing
from sklearn import svm, grid_search
from sklearn.feature_selection import SelectFromModel as select
import numpy as np
import pandas as pd

df, salary, keys = CLF.clean(CLF.get_data())
df = preprocessing.StandardScaler().fit(df).transform(df)
df = pd.DataFrame(df, columns=CLF.get_features())

X_test, y_test, keys = CLF.clean(CLF.test_data())
X_test = preprocessing.StandardScaler().fit(X_test).transform(X_test)
X_test = pd.DataFrame(X_test, columns=CLF.get_features())


splits = [0.4,0.33,0.25]
param_grid = [
      {
          'C': [0.1, 1, 10, 100],
          'max_iter': [1000, 10000],
          'kernel': ['linear']
      },
      {
          'C': [0.1, 1, 10, 100],
          'gamma': [0.1, 0.01, 0.001, 0.0001],
          'kernel': ['rbf']
      },
]
clf = svm.SVC()
clf = grid_search.GridSearchCV(clf, param_grid, n_jobs=-1)

tests = []
for split in splits:
    X_train, X_val, y_train, y_val = CLF.split_data(df, salary, split, 42)
    tests.append(clf.fit(X_train, y_train))
