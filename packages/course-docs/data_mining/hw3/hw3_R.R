# load the data
data = read.csv('prostate.csv')

# split the dataset into train and test. 

sub <- sample(nrow(data), floor(nrow(data) * 0.66)) # 1/3 testing data
train.x <- as.matrix(data[sub, -c(1) ])
test.x <- as.matrix(data[-sub, -c(1)])
train.y <- data[sub, 1 ]
test.y <- data[-sub, 1]

# load the glment library
library(glmnet)

# if alpha.value=0 then use ridge regression to fit a model
# if alpha.value=1 then use lasso regression to fit a model
fitModel <- function(train.x, train.y, test.x, test.y, alpha.value, type="") {  
	# fit models using the functions in the glmnet package
	 model <- glmnet(train.x, train.y, alpha=alpha.value);
      model.cv <- cv.glmnet(train.x, train.y, alpha=alpha.value, nfold=5);
	#
	if (alpha.value == 0){
	model.cv <- cv.glmnet(train.x, train.y, alpha=alpha.value, nfold=5,lambda = c(0.00001, 0.0001,0.001, 0.005, 0.01, 0.05, 0.1, 1, 5, 10, 100));}
     
	if (alpha.value == 1){
	model.cv <- cv.glmnet(train.x, train.y, alpha=alpha.value, nfold=5,lambda = c(0.0001, 0.0005, 0.001, 0.005, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5));}

	# plot cross-validation profiles
	pdf(paste(type, 'cross_valid.pdf', sep='-'));
	plot(model.cv);
	dev.off();
	
	# plot the coefficient paths
	pdf(paste(type, 'coef_path.pdf', sep='-'));
	plot(model, xvar="lambda");
	dev.off();
	
	# make predictions on the test data
	predicted.value <- predict(model.cv, newx = test.x, s = "lambda.min");
	
	# returns the mean squared error and the optimal lambda value
	return(list(mean((predicted.value - test.y)^2), model.cv$lambda.min, model.cv));
}

# fit a model using ridge regression and assess the goodness of fit
ridge.model <- fitModel(train.x, train.y, test.x, test.y, 0, "ridge");

# fit a model using lasso regression and assess the goodness of fit
lasso.model <- fitModel(train.x, train.y, test.x, test.y, 1, "lasso");

# coefficients for the optimal lambda lasso
#coef(lasso.model[[3]])
coef(lasso.model[[3]],s=lasso.model[[2]])
# seems like different features are dropped in different runs
#lasso.model <- glmnet(train.x, train.y, alpha = 1,lambda = lasso.model[[3]]$lambda.min)
#coef(lasso.model) 

# fit a model using the lm function (no regularization)
lm.model <- lm(train.y~., data=as.data.frame(train.x));

# compute the MSE and optimal lambda on the test data for the above model
predicted.value <- predict(lm.model, as.data.frame(test.x));
mse <- data.frame(type=c("MLR", "Ridge", "Lasso"),
	lambda=c(0, ridge.model[[2]], lasso.model[[2]]),
	mse=c(mean((predicted.value - test.y)^2), ridge.model[[1]], lasso.model[[1]]))


# create the new model after dropping features; change the features accordingly
update.mlr.model <- lm(train.y~lweight+age+lbph+svi+lcp+lpsa, 
	data=as.data.frame(train.x));
coef(update.mlr.model)

## calculate the new mse
predicted.value <- predict(update.mlr.model, as.data.frame(test.x));
mse <- rbind(mse, data.frame(type="Re-run MLR", lambda=0, mse=mean((predicted.value - test.y)^2)))

library(xtable)
print(xtable(mse), include.rownames=FALSE)

print(xtable(as.matrix(coef(lasso.model[[3]], s="lambda.min")), digits=5))




