import pandas as pd
from sklearn.preprocessing import OneHotEncoder as OHE
from sklearn import cross_validation as cv
from sklearn import svm
from sklearn.neighbors import KNeighborsClassifier as KNC

df = pd.read_csv("../dataset/train.csv")
y = df['ACTION']
del df['ACTION']

X_test = pd.read_csv("../dataset/test.csv")
employee_id = X_test['id']
del X_test['id']

X = df[(df['RESOURCE']==X_test['RESOURCE'])].dropna(how='all')

n_list = []
for column in X.columns:
    n_list.append(X[column].value_counts().size)
n2_list = []
for column in X_test.columns:
    n2_list.append(X_test[column].value_counts().size)

enc = OHE()
X = enc.fit_transform(X)
X_test = enc.fit_transform(X_test)

X_train, X_val, y_train, y_val = cv.train_test_split(
    X, y, test_size=0.4, random_state=0
)
linSVC = svm.LinearSVC()
prediction = linSVC.fit(X, y).predict(X_test)
