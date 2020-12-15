from pyspark import SparkContext, RDD, sql
from pyspark.mllib.regression import LabeledPoint
from pyspark.mllib.tree import DecisionTree
from pyspark.mllib.tree import RandomForest
import pandas as pd

sc = SparkContext(appName="DecisionTreeClassification")
sc.setLogLevel("ERROR")

data = pd.read_table("../dataset/arcene_train.data", sep=" ", header=None)
del data[10000]
target = pd.read_table("../dataset/arcene_train.labels", sep=" ", header=None).replace(to_replace="-1", value="0")
labeled_point_array = []
df_arrays = data.values
for index, row in data.iterrows():
        lp = LabeledPoint(target[0][index], df_arrays[index])
        labeled_point_array.append(lp)
rdd = sc.parallelize(labeled_point_array)
(trainingData, validationData) = rdd.randomSplit([0.7, 0.3])

def testModel(model, validationData):
    predictions = model.predict(validationData.map(lambda x: x.features))
    labelsAndPredictions = validationData.map(lambda lp: lp.label).zip(predictions)
    testErr = labelsAndPredictions.filter(lambda (v, p): v != p).count() / float(validationData.count())
    testAccuracy = 1-testErr
    print('Validation Error = ' + str(testErr))
    print('Learned classification tree model:')
    print(model.toDebugString())
    return testAccuracy

models = list()
accuracies = list()
impurities = ['gini', 'entropy']
mBins = [16, 32, 48, 64, 80, 96, 112, 128, 144, 160]
mDepth = [1, 2, 3, 4, 5]

for i in impurities:
    models.append(DecisionTree.trainClassifier(trainingData, numClasses=2, categoricalFeaturesInfo={}, impurity=i, maxDepth=5, maxBins=32))
    accuracies.append(testModel(models[len(models)-1], validationData))
for m in mBins:
    models.append(DecisionTree.trainClassifier(trainingData, numClasses=2, categoricalFeaturesInfo={}, impurity='gini', maxDepth=5, maxBins=m))
    accuracies.append(testModel(models[len(models)-1], validationData))
for m in mDepth:
    models.append(DecisionTree.trainClassifier(trainingData, numClasses=2, categoricalFeaturesInfo={}, impurity='gini', maxDepth=m, maxBins=32))
    accuracies.append(testModel(models[len(models)-1], validationData))



