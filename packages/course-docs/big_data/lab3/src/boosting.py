import SVM as CLF
import numpy as np
import matplotlib.pyplot as plt
from sklearn.cross_validation import cross_val_score as cvs
from sklearn.ensemble import AdaBoostClassifier as ABC

df, salary, keys = CLF.clean(CLF.get_data())
estimators = [10, 20, 30, 40, 50, 100, 200, 400]
estimator_scores = []
for estimator in estimators:
    clf = ABC(n_estimators=estimator)
    estimator_scores.append(cvs(clf, df, salary).mean())

learning_rates = [1, 10, 20, 30, 40, 50, 100, 200]
learning_scores = []
best_estimator = estimators[estimator_scores.index(max(estimator_scores))]
for rate in learning_rates:
    clf = ABC(n_estimators=best_estimator, learning_rate=rate)
    learning_scores.append(cvs(clf, df, salary).mean())

n_estimators = 400
# A learning rate of 1. may not be optimal for both SAMME and SAMME.R
learning_rate = 1.

fig = plt.figure()
ax = fig.add_subplot(111)


