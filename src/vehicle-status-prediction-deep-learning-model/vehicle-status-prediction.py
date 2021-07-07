import pandas as pd

# df = pd.read_csv('vehicle-status-prediction-data.csv')
df = pd.read_csv('test-data.csv')
noOfcells = df['NoOfCells'][0]

# inputColumn = list(df)[2:14]
# print(inputColumn)
# input_X_df = df[inputColumn].astype(float)
# print(input_X_df.head(5))

# input_Y_df = df[list(df)[14:]].astype(float)
# print(input_Y_df.head(5))

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

    elif index % noOfcells == 0:
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

print(input_X)
print(input_Y)