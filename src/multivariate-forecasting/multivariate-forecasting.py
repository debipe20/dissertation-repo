import numpy as np
from keras.models import Sequential
from keras.layers import LSTM
from keras.layers import Dense
from keras.layers import Dropout
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
import seaborn as sns

df = pd.read_csv('GE.csv')

#seperate dates for future plotting
train_dates = pd.to_datetime(df['Date'])

#variable for training
cols = list(df)[1:6]

df_for_training = df[cols].astype(float)
df_for_plot = df_for_training.tail(5000)
df_for_plot.plot.line()

#LSTM uses sigmoid and tanh that are sensitive to magnitude so values need to be normalized
#normalize the dataset
scaler = StandardScaler()
scaler = scaler.fit(df_for_training)
df_for_training_scaled = scaler.transform(df_for_training)


#As required for LSTM networks, we require to reshape an input data into n_samples x timestep
#In this example, the n_features is 2. We will make timesteps = 3
#With this, the resultant n_samples is 5 (as the input data has 9 rows)
trainX = []
trainY = []

n_future = 1 #Number of days we want to predict into the future
n_past = 14 #Number of past days we want to use to predict the future

for i in range (n_past, len(df_for_training_scaled) - n_future + 1):
    trainX.append(df_for_training_scaled[i - n_past:i, 0:df_for_training.shape[1]])
    trainY.append(df_for_training_scaled[i + n_future -1:i +n_future, 0])
    
trainX, trainY = np.array(trainX), np.array(trainY)

print('trainX shape == {}.'.format(trainX.shape))
print('trainY shape == {}.'.format(trainY.shape))

#define Autoencoder model

model = Sequential()
model.add(LSTM(64, activation = 'relu', input_shape =(trainX.shape[1],trainX.shape[2]), return_sequences = True))
model.add(LSTM(32, activation = 'relu', return_sequences = False))
model.add(Dropout(0.2))
# Adding the output layer
model.add(Dense(trainY.shape[1]))

model.compile(optimizer = 'adam', loss = 'mse')
model.summary()

#fit model

history = model.fit(trainX, trainY, epochs =10, batch_size = 16, validation_split= 0.1, verbose =1)

plt.plot(history.history['loss'], label ="Training Loss")
plt.plot(history.history['val_loss'], label ="Validation Loss")
plt.legend()

#Forecasting

n_future = 90

forecast_period_dates = pd.date_range(list(train_dates)[-1], periods=n_future, freq='1d').tolist()
forecast = model.predict(trainX[-n_future:])
#We have 5 variables for transform, the inverse expects same dimensions
forecast_copies = np.repeat(forecast, df_for_training.shape[1], axis = -1)
y_pred_future = scaler.inverse_transform(forecast_copies)[:,0]





