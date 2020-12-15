from sklearn import decomposition
import pandas
import matplotlib.pyplot as plt

datapath = '../../data/'
processeddata = '../../data/processed/'

data = pandas.read_csv(processeddata + 'matt_processed_results.csv')

drop_features = ['Season', 'Team', 'AvgPointSpread', 'Wins', 'Losses']
X = data.drop(drop_features, axis=1)
y = data['Wins']

# print(X.head())
# print(y.head())

# FieldGoalAvg	ThreePointAvg	FreeThrowAvg	ReboundAvg	AssistAvg	TurnOverAvg	StealAvg	BlockAvg	PersonalFoulAvg
pca = decomposition.PCA()
pca.fit(X)

fig = plt.figure()
plt.title('Scree Plot')
ax = fig.add_subplot(111)
ax.plot(pca.explained_variance_ratio_, linewidth=2)
# plt.show()

c = pca.components_
print(c)

fig = plt.figure()
width = 0.25
ind = list(range(0,9))
colors = ['r', 'g', 'b']
for i in range(0,3):
    ax = fig.add_subplot(111)
    ax.bar([x + width*i for x in ind], c[i], width=width, color=colors[i])
ax.set_xticklabels(X.columns.values, rotation=45, ha='left', size='large')
plt.title('Top 3 PCA by Weights')

# ax = fig.add_subplot(211)
# ax.bar(range(0,9), c[1])
# ax.set_xticklabels(X.columns.values, rotation=45, ha='left')

plt.show()


