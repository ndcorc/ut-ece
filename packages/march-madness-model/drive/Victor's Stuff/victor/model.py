from sklearn import linear_model
from sklearn.cross_validation import train_test_split
from sklearn.grid_search import GridSearchCV
from sklearn import ensemble
import pandas
import matplotlib.pyplot as plt

def score_model(model, parameters, X_train, y_train, X_test, y_test):
    m = model()
    m = GridSearchCV(m, parameters, cv=3)
    m.fit(X_train, y_train)
    score = m.score(X_test, y_test)
    return score

datapath = '../../data/'
processeddata = '../../data/processed/'

data = pandas.read_csv(processeddata + 'matt_processed_results.csv')

features = ['FieldGoalAvg','ThreePointAvg','FreeThrowAvg','ReboundAvg','AssistAvg','TurnOverAvg','StealAvg','BlockAvg','PersonalFoulAvg']
features = ['ReboundAvg', 'AssistAvg', 'PersonalFoulAvg']
X = data[features]
y = data['Wins']

# print(X.head())
# print(y.head())

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.33, random_state=42)

p = {'normalize':[True, False]}
s = score_model(linear_model.LinearRegression, p, X_train, y_train, X_test, y_test)
print('Linear Regression Score: ', s)

p = {'penalty':['l1', 'l2'], 'C':[0.001, 0.01, 0.1, 1, 10, 100, 1000], 'solver':['liblinear']}
s = score_model(linear_model.LogisticRegression, p, X_train, y_train, X_test, y_test)
print('Logistic Regression Score: ', s)

p = {'loss':['squared_loss', 'huber', 'epsilon_insensitive', 'squared_epsilon_insensitive'], 'penalty':['none', 'l2', 'l1', 'elasticnet'], 'n_iter':[1,3,5,7,9]}
s = score_model(linear_model.SGDRegressor, p, X_train, y_train, X_test, y_test)
print('SGD Regressor Score: ', s)

p = {'n_estimators':[10, 50, 100, 500, 1000, 5000]}
s = score_model(ensemble.RandomForestRegressor, p, X_train, y_train, X_test, y_test)
print('Random Forest Regressor Score: ', s)