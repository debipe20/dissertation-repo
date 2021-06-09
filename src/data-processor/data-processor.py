import pandas as pd 


def findUniqueVehicleIdInDataFrame(dataframe):
    uniqueVehicleId = []
    for idx, row in dataframe.loc[:].iterrows():
        if row['VehicleId'] not in uniqueVehicleId:
            uniqueVehicleId.append(row['VehicleId'])

    return uniqueVehicleId


def main():
    df = pd.read_csv('data.csv')
    uniqueVehicleId = findUniqueVehicleIdInDataFrame(df)
    print(uniqueVehicleId)    
    
if __name__ == "__main__":
    main()   