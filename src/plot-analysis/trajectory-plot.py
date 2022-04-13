from datetime import time
import numpy as np
import pandas as pd
import json
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle


def timeSpaceDiagram(connectedVehicleTimePoint, nonConnectedVehicleTimePoint, connectedVehicleDistancePoint, nonConnectedVehicleDistancePoint, approachLength, timeLength, greenRectangleStartPoint, greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime, startTime, endTime):
    fig, ax1 = plt.subplots()

    ax1.set_xlabel('Time (s)', fontsize=24, fontweight='bold')
    ax1.set_ylabel('Distance (m)', fontsize=24, fontweight='bold')

    plt.xlim([0, timeLength])
    plt.ylim([0, approachLength+50])
    plt.xticks(np.arange(0, timeLength+10, 25), fontsize=24)
    plt.yticks(np.arange(0, approachLength+50, 50), fontsize=24)

    # Plot Phase Status
    req_phase_length = len(greenRectangleStartPoint)
    for i in range(0, req_phase_length):
        x = greenRectangleStartPoint[i]
        y = approachLength
        ax1.add_patch(Rectangle(
            (x, y), greenRectangleTime[i], 10, angle=0.0, color='green', linewidth=2,))

    req_phase_length = len(clearanceRectangleStartPoint)
    for i in range(0, req_phase_length):
        x = clearanceRectangleStartPoint[i]
        y = approachLength
        ax1.add_patch(Rectangle(
            (x, y), clearanceRectangleTime[i], 10, angle=0.0, color='red', linewidth=2))

    # Plot VehicleTrajectory
    if len(connectedVehicleTimePoint) > 0:
        ax1.scatter(connectedVehicleTimePoint, connectedVehicleDistancePoint, c="blue",  linewidths=4,
                    marker=".",  edgecolor="none",  s=50, label='Connected Vehicles Trajectory', zorder=2)

    if len(nonConnectedVehicleTimePoint) > 0:
        ax1.scatter(nonConnectedVehicleTimePoint, nonConnectedVehicleDistancePoint, c="gray",  linewidths=4,
                    marker=".",  edgecolor="none",  s=20, label='Non-Connected Vehicles Trajectory', zorder=2)

    ax1.legend(loc='upper right', prop={"size": 14})
    ax1.set_title("Time-Space-Trajectory Diagram [" + str(startTime) + ", " + str(endTime) + "]", fontsize=20, fontweight='bold')
    fig.tight_layout()  # otherwise the right y-label is slightly clipped
    plt.grid(color='black', linestyle='-', linewidth=0.5)
    plt.show()


def getPhaseStatusPoint(dataFrame, startTime, laneId):

    greenRectangleStartPoint = []
    greenRectangleTime = []
    clearanceRectangleStartPoint = []
    clearanceRectangleTime = []

    currentPhaseStatusStartTime = startTime
    dataFrame = dataFrame.loc[dataFrame["LaneId"] == laneId]
    dataFrame = dataFrame.reset_index(drop=True)
    dataFrame["PhaseStatus"].replace({2: 4, 3: 4}, inplace=True)
    currentPhaseStatus = dataFrame['PhaseStatus'][0]


    if currentPhaseStatus == 1:
        greenRectangleStartPoint.append(0.0)

    else:
        clearanceRectangleStartPoint.append(0.0)

    for idx, row in dataFrame.loc[:].iterrows():
        if currentPhaseStatus != row['PhaseStatus']:
            if row['PhaseStatus'] == 1:
                greenRectangleStartPoint.append(row['TimeStamp'] - startTime)
                clearanceRectangleTime.append(
                    row['TimeStamp'] - currentPhaseStatusStartTime)
                currentPhaseStatus = row['PhaseStatus']
                currentPhaseStatusStartTime = row['TimeStamp']

            else:
                clearanceRectangleStartPoint.append(
                    row['TimeStamp'] - startTime)
                greenRectangleTime.append(
                    row['TimeStamp'] - currentPhaseStatusStartTime)
                currentPhaseStatus = row['PhaseStatus']
                currentPhaseStatusStartTime = row['TimeStamp']

    if len(greenRectangleStartPoint) > len(greenRectangleTime):
        greenRectangleTime.append(
            dataFrame['TimeStamp'].iloc[-1] - currentPhaseStatusStartTime + 10)

    if len(clearanceRectangleStartPoint) > len(clearanceRectangleTime):
        clearanceRectangleTime.append(
            dataFrame['TimeStamp'].iloc[-1] - currentPhaseStatusStartTime + 10)

    return greenRectangleStartPoint, greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime

#Method 1
def getTrajectoryPoint(dataFrame, startTime, approachLength, estimatedData):
    connectedVehicleTimePoint = []
    nonConnectedVehicleTimePoint = []
    connectedVehicleDistancePoint = []
    nonConnectedVehicleDistancePoint = []

    for idx, row in dataFrame.loc[:].iterrows():
        # For Estimated Data
        if bool(estimatedData):
            if row['CellStatus'] == 1 and row['ConnectedVehicleId'] > 0:
                connectedVehicleTimePoint.append(row['TimeStamp'] - startTime)
                connectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])

            elif row['CellStatus'] == 1 and row['PredictedCellStatus'] > 0.05 and row['NonConnectedVehicleId'] > 0:
                nonConnectedVehicleTimePoint.append(
                    row['TimeStamp'] - startTime)
                nonConnectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])

        # For Sample Data
        else:
            if row['CellStatus'] == 1 and row['ConnectedVehicleId'] > 0:
                connectedVehicleTimePoint.append(row['TimeStamp'] - startTime)
                connectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])

            elif row['CellStatus'] == 1 and row['NonConnectedVehicleId'] > 0:
                nonConnectedVehicleTimePoint.append(
                    row['TimeStamp'] - startTime)
                nonConnectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])

    return connectedVehicleTimePoint, nonConnectedVehicleTimePoint, connectedVehicleDistancePoint, nonConnectedVehicleDistancePoint

#Method 2
def getVehicleTrajectoryPoint(dataFrame, startTime, endTime, laneId, approachLength, estimatedData):

    connectedVehicleTimePoint = []
    connectedVehicleDistancePoint = []
    nonConnectedVehicleTimePoint = []
    nonConnectedVehicleDistancePoint = []

    uniqueConnectedVehicleId = findUniqueConnectedVehicleIdInDataFrame(
        dataFrame, startTime, endTime, laneId)
    
    print("Connected Vehicles Id: ", uniqueConnectedVehicleId)
    print("No of Connected Vehicles: ", len(uniqueConnectedVehicleId))
    
    uniqueNonConnectedVehicleId = findUniqueNonConnectedVehicleIdInDataFrame(
        dataFrame, startTime, endTime, laneId)

    print("Non-Connected Vehicles Id: ", uniqueNonConnectedVehicleId)
    print("No of Non-Connected Vehicles:", len(uniqueNonConnectedVehicleId))

    for vehicleId in uniqueConnectedVehicleId:
        connectedVehicleTimePointList, connectedVehicleDistancePointList = getConnectedVehicleTrajectory(
            dataFrame, vehicleId, laneId, approachLength, startTime, endTime, estimatedData)
        [connectedVehicleTimePoint.append(element)
         for element in connectedVehicleTimePointList]
        [connectedVehicleDistancePoint.append(
            element) for element in connectedVehicleDistancePointList]

    for vehicleId in uniqueNonConnectedVehicleId:
        nonConnectedVehicleTimePointList, nonConnectedVehicleDistancePointList = getNonConnectedVehicleTrajectory(
            dataFrame, vehicleId, laneId, approachLength, startTime, endTime, estimatedData)
        [nonConnectedVehicleTimePoint.append(
            element) for element in nonConnectedVehicleTimePointList]
        [nonConnectedVehicleDistancePoint.append(
            element) for element in nonConnectedVehicleDistancePointList]

    return connectedVehicleTimePoint, nonConnectedVehicleTimePoint, connectedVehicleDistancePoint, nonConnectedVehicleDistancePoint


def findUniqueConnectedVehicleIdInDataFrame(bsmDf, startTime, endTime, laneId):
    uniqueConnectedVehicleId = []

    if not bsmDf.empty:
        for idx, row in bsmDf.loc[:].iterrows():
            if row['ConnectedVehicleId'] not in uniqueConnectedVehicleId and row['TimeStamp'] >= startTime and row['TimeStamp'] <= endTime and row['LaneId'] == laneId:
                uniqueConnectedVehicleId.append(row['ConnectedVehicleId'])

    uniqueConnectedVehicleId.remove(0)

    return uniqueConnectedVehicleId


def findUniqueNonConnectedVehicleIdInDataFrame(bsmDf, startTime, endTime, laneId):
    uniqueNonConnectedVehicleId = []

    if not bsmDf.empty:
        for idx, row in bsmDf.loc[:].iterrows():
            if row['NonConnectedVehicleId'] not in uniqueNonConnectedVehicleId and row['TimeStamp'] >= startTime and row['TimeStamp'] <= endTime and row['LaneId'] == laneId:
                uniqueNonConnectedVehicleId.append(
                    row['NonConnectedVehicleId'])

    uniqueNonConnectedVehicleId.remove(0)
    return uniqueNonConnectedVehicleId


def getConnectedVehicleTrajectory(dataFrame, vehicleId, laneId, approachLength, startTime, endTime, estimatedData):

    previousStartTime = startTime - 1.0
    connectedVehicleTimePoint = []
    connectedVehicleDistancePoint = []

    dataFrame = dataFrame.loc[dataFrame["ConnectedVehicleId"] == vehicleId]

    for idx, row in dataFrame.loc[:].iterrows():
        # For Estimated Data
        if bool(estimatedData):
            if row['CellStatus'] == 1 and row['TimeStamp'] - previousStartTime >= 0.3 and row['TimeStamp'] >= startTime and row['TimeStamp'] <= endTime and row['LaneId'] == laneId:
                connectedVehicleTimePoint.append(row['TimeStamp'] - startTime)
                connectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])
                previousStartTime = row['TimeStamp']

        # For Sample Data
        else:
            if row['CellStatus'] == 1 and row['TimeStamp'] - previousStartTime >= 0.3 and row['TimeStamp'] >= startTime and row['TimeStamp'] <= endTime and row['LaneId'] == laneId:
                connectedVehicleTimePoint.append(row['TimeStamp'] - startTime)
                connectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])

    return connectedVehicleTimePoint,  connectedVehicleDistancePoint


def getNonConnectedVehicleTrajectory(dataFrame, vehicleId, laneId, approachLength, startTime, endTime, estimatedData):
    previousStartTime = startTime - 1.0
    nonConnectedVehicleTimePoint = []
    nonConnectedVehicleDistancePoint = []
    dataFrame = dataFrame.loc[dataFrame["NonConnectedVehicleId"] == vehicleId]

    for idx, row in dataFrame.loc[:].iterrows():
        # For Estimated Data
        if bool(estimatedData):
            if row['CellStatus'] == 1 and row['PredictedCellStatus'] > 0.05 and row['TimeStamp'] - previousStartTime >= 0.3 and row['TimeStamp'] >= startTime and row['TimeStamp'] <= endTime and row['LaneId'] == laneId:
                nonConnectedVehicleTimePoint.append(
                    row['TimeStamp'] - startTime)
                nonConnectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])
                previousStartTime = row['TimeStamp']

        # For Sample Data
        else:
            if row['CellStatus'] == 1 and row['TimeStamp'] - previousStartTime >= 0.3 and row['TimeStamp'] >= startTime and row['TimeStamp'] <= endTime and row['LaneId'] == laneId:
                nonConnectedVehicleTimePoint.append(
                    row['TimeStamp'] - startTime)
                nonConnectedVehicleDistancePoint.append(
                    approachLength - row['DistanceToStopBar'])

                previousStartTime = row['TimeStamp']

    return nonConnectedVehicleTimePoint, nonConnectedVehicleDistancePoint


def main():
    # Read the config file into a json object:
    configFile = open("configuration.json", 'r')
    config = (json.load(configFile))
    # Close the config file:
    configFile.close()

    dataFrame = pd.read_csv(config["FileName"])
    laneId = config["LaneId"]
    approachLength = config["ApproachLength"]
    estimatedData = config["EstimatedData"]

    startTime = dataFrame['TimeStamp'][0]
    endTime = dataFrame['TimeStamp'].iloc[-1]
    
    startTime = config["StartTimeOfDiagram"]
    endTime = config["EndTimeOfDiagram"]

    timeLength = endTime - startTime
    print("Start Time is:", startTime)

    connectedVehicleTimePoint, nonConnectedVehicleTimePoint, connectedVehicleDistancePoint, nonConnectedVehicleDistancePoint = getTrajectoryPoint(
        dataFrame, startTime, approachLength, estimatedData)

    # connectedVehicleTimePoint, nonConnectedVehicleTimePoint, connectedVehicleDistancePoint, nonConnectedVehicleDistancePoint = getVehicleTrajectoryPoint(
    #     dataFrame, startTime, endTime, laneId, approachLength, estimatedData)

    greenRectangleStartPoint, greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime = getPhaseStatusPoint(
        dataFrame, startTime, laneId)

    print("Iteration is done")

    timeSpaceDiagram(connectedVehicleTimePoint, nonConnectedVehicleTimePoint, connectedVehicleDistancePoint, nonConnectedVehicleDistancePoint, approachLength, timeLength, greenRectangleStartPoint,
                     greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime, startTime, endTime)


if __name__ == "__main__":
    main()
