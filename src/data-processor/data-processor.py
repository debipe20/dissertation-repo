import numpy as np
from keras.models import Sequential
from keras.layers import LSTM
from keras.layers import Dense
from keras.layers import Dropout
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import StandardScaler
import seaborn as sns


def findUniqueVehicleIdInDataFrame(dataframe):
    uniqueVehicleId = []
    for idx, row in dataframe.loc[:].iterrows():
        if row['VehicleId'] not in uniqueVehicleId:
            uniqueVehicleId.append(row['VehicleId'])

    return uniqueVehicleId


def main():
    df = pd.read_csv('test-data.csv')
    print("Number of rows and columns:", df.shape)

    uniqueVehicleId = findUniqueVehicleIdInDataFrame(df)
    df['VehicleType'] = df['VehicleType'].replace(['Transit'],6)
    df['VehicleType'] = df['VehicleType'].replace(['Truck'],9)
    df['VehicleType'] = df['VehicleType'].replace(['EmergencyVehicle'],2)
    df['VehicleType'] = df['VehicleType'].replace(['Car'],4)
    df['LocationOnMap'] = df['LocationOnMap'].replace(['inbound'],2)
    df['LocationOnMap'] = df['LocationOnMap'].replace(['outbound'],4)
    df['LocationOnMap'] = df['LocationOnMap'].replace(['insideIntersectionBox'],1)
    cols = list(df)[1:10]    
    df_for_training = df[cols].astype(float)
    #LSTM uses sigmoid and tanh that are sensitive to magnitude so values need to be normalized
    #normalize the dataset
    scaler = StandardScaler()
    # scaler = scaler.fit(df_for_training)
    # df_for_training_scaled = scaler.transform(df_for_training)
    trainX = []
    trainY = []
    n_future = 1 #Number of days we want to predict into the future
    n_past = 4 #Number of past days we want to use to predict the future
    
    
    for vehicleId in uniqueVehicleId:
        #x = len(df[df['VehicleId'] ==  vehicleId])

        temporaryDataframe = str(vehicleId)
        temporaryDataframe = df_for_training.loc[df_for_training['VehicleId'] == vehicleId]
        print(temporaryDataframe.head(5))
        scaler = scaler.fit(temporaryDataframe)
        temporaryDataframe_scaled = scaler.transform(temporaryDataframe)
        print( temporaryDataframe_scaled)
        temporaryTrainX = []
        temporaryTrainY = []
        for i in range (n_past, len(temporaryDataframe_scaled) - n_future + 1):
            trainX.append(temporaryDataframe_scaled[i - n_past:i, 0:temporaryDataframe_scaled.shape[1]])
            trainY.append(temporaryDataframe_scaled[i + n_future -1:i +n_future, 0])
    #     print(temporaryTrainX)
    #     for element in temporaryTrainX:
    #         trainX.append(temporaryTrainX)
    #     for element in temporaryTrainY:
    #         trainY.append(temporaryTrainY)
            
    #         temporaryTrainX.append(temporaryDataframe_scaled[i - n_past:i, 0:temporaryDataframe_scaled.shape[1]])
    #         temporaryTrainY.append(temporaryDataframe_scaled[i + n_future -1:i +n_future, 0])
    #     print(temporaryTrainX)
    #     for element in temporaryTrainX:
    #         trainX.append(temporaryTrainX)
    #     for element in temporaryTrainY:
    #         trainY.append(temporaryTrainY)
    trainX, trainY = np.array(trainX), np.array(trainY)
    print(trainX)
    print('trainX shape == {}.'.format(trainX.shape))
    print('trainY shape == {}.'.format(trainY.shape))
    
    
    # print(uniqueVehicleId)    
    
if __name__ == "__main__":
    main()   