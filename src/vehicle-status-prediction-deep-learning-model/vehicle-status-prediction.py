import pandas as pd
import numpy as np
from keras.models import Sequential
from keras.layers import Dense, Conv2D, Flatten 
from keras.layers import MaxPooling2D, Dropout

from sklearn.model_selection import train_test_split

def getTrainingSetSize(df, noOfcells):
    trainingDataPointSize = int(df.shape[0]*0.8/noOfcells)
    training_setsize = trainingDataPointSize*noOfcells
    
    return training_setsize
def getProcessedDataSet(df):
    input_X = []
    input_Y = []
    dataPoint_X = []
    dataPoint_Y = []

    for index, rows in df.iterrows():
        if index == 0:
            X_row_list = [rows.InputVehicleId, rows.InputVehicleType, rows.InputSignalGroup, rows.InputLaneId, rows.InputApproachId, rows.InputLocationOnMap,
                    rows.InputPhaseStatus, rows.InputSpeed, rows.InputHeading, rows.InputDistanceToStopBar, rows.InputStoppedDelay, rows.InputCellStatus]
            
            Y_row_list = [rows.OutputVehicleId, rows.OutputVehicleType, rows.OutputSignalGroup, rows.OutputLaneId, rows.OutputApproachId, rows.OutputLocationOnMap,
                    rows.OutputPhaseStatus, rows.OutputSpeed, rows.OutputHeading, rows.OutputDistanceToStopBar, rows.OutputStoppedDelay, rows.OutputCellStatus]
            
            dataPoint_X.append(X_row_list)
            dataPoint_Y.append(Y_row_list)
        
        elif index % noOfcells != 0:
            X_row_list = [rows.InputVehicleId, rows.InputVehicleType, rows.InputSignalGroup, rows.InputLaneId, rows.InputApproachId, rows.InputLocationOnMap,
                    rows.InputPhaseStatus, rows.InputSpeed, rows.InputHeading, rows.InputDistanceToStopBar, rows.InputStoppedDelay, rows.InputCellStatus]
            
            Y_row_list = [rows.OutputVehicleId, rows.OutputVehicleType, rows.OutputSignalGroup, rows.OutputLaneId, rows.OutputApproachId, rows.OutputLocationOnMap,
                    rows.OutputPhaseStatus, rows.OutputSpeed, rows.OutputHeading, rows.OutputDistanceToStopBar, rows.OutputStoppedDelay, rows.OutputCellStatus]
            
            dataPoint_X.append(X_row_list)
            dataPoint_Y.append(Y_row_list)

        elif index % noOfcells == 0 and len(dataPoint_X) > 0:
            input_X.append(dataPoint_X)
            input_Y.append(dataPoint_Y)
            
            dataPoint_X = []
            dataPoint_Y = []

            X_row_list = [rows.InputVehicleId, rows.InputVehicleType, rows.InputSignalGroup, rows.InputLaneId, rows.InputApproachId, rows.InputLocationOnMap,
                    rows.InputPhaseStatus, rows.InputSpeed, rows.InputHeading, rows.InputDistanceToStopBar, rows.InputStoppedDelay, rows.InputCellStatus]
            
            Y_row_list = [rows.OutputVehicleId, rows.OutputVehicleType, rows.OutputSignalGroup, rows.OutputLaneId, rows.OutputApproachId, rows.OutputLocationOnMap,
                    rows.OutputPhaseStatus, rows.OutputSpeed, rows.OutputHeading, rows.OutputDistanceToStopBar, rows.OutputStoppedDelay, rows.OutputCellStatus]
            
            dataPoint_X.append(X_row_list)
            dataPoint_Y.append(Y_row_list)
        
        if index == len(df) - 1:
            input_X.append(dataPoint_X)
            input_Y.append(dataPoint_Y)  


    input_X = np.array(input_X)
    input_Y = np.array(input_Y)
    input_X = input_X.reshape(input_X.shape[0], input_X.shape[1], input_X.shape[2], 1)
    input_Y = input_X.reshape(input_Y.shape[0], input_Y.shape[1], input_Y.shape[2], 1)
    
    return input_X, input_Y

df = pd.read_csv('vehicle-status-prediction-data.csv')
# df = pd.read_csv('test-data.csv')
noOfcells = df['NoOfCells'][0]

# inputColumn = list(df)[2:14]
# print(inputColumn)
# input_X_df = df[inputColumn].astype(float)
# print(input_X_df.head(5))

# input_Y_df = df[list(df)[14:]].astype(float)
# print(input_Y_df.head(5))
print(df.shape[0])
training_setsize = getTrainingSetSize(df, noOfcells)
df_forTraining = df.iloc[:training_setsize, 1:]
df_forTesting = df.iloc[training_setsize:, 1:]

df_forTesting.reset_index(inplace=True)
print(df_forTesting.head())

trainX, trainY = getProcessedDataSet(df_forTraining)
testX, testY = getProcessedDataSet(df_forTesting)
input_shape = (trainX.shape[1], trainX.shape[2], 1)

model = Sequential()#add model layers
model.add(Conv2D(32, kernel_size=(5, 5),
                     activation='relu',
                     input_shape=input_shape))
model.add(MaxPooling2D(pool_size=(2, 2)))
# add second convolutional layer with 20 filters
# model.add(Conv2D(64, (5, 5), activation='relu'))
    
# add 2D pooling layer
model.add(MaxPooling2D(pool_size=(2, 2)))
    
# flatten data
model.add(Flatten())
    
# add a dense all-to-all relu layer
model.add(Dense(1024, activation='relu'))
    
# apply dropout with rate 0.5
model.add(Dropout(0.5))
    
# soft-max layer
# model.add(Dense(156, activation='softmax'))#compile model using accuracy to measure model performance
model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

#train the model
model.fit(trainX, trainY, validation_data=(testX, testY), epochs=3)# evaluate the model
score = model.evaluate(testX, testY, verbose=1)# print performance
print()
print('Test loss:', score[0])
print('Test accuracy:', score[1])