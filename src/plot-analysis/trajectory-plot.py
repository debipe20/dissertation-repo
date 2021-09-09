from datetime import time
import numpy as np
import pandas as pd
import json
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle


def timeSpaceDiagram(timePoint, distancePoint, approachLength, timeLength, greenRectangleStartPoint, greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime):
    fig, ax1 = plt.subplots()

    ax1.set_xlabel('Time (s)', fontsize=24, fontweight='bold')
    ax1.set_ylabel('Distance (m)', fontsize=24, fontweight='bold')

    plt.xlim([0, timeLength])
    plt.ylim([0, approachLength+50])
    plt.xticks(np.arange(0, timeLength+10, 10), fontsize=24)
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
    if len(timePoint) > 0:
        ax1.scatter(timePoint, distancePoint, c="black",  linewidths=4,
                    marker=".",  edgecolor="none",  s=50, label='Vehicle Trajectory', zorder=2)

    ax1.legend(loc='upper right', prop={"size": 16})
    ax1.set_title("Time-Space Diagram", fontsize=20, fontweight='bold')
    fig.tight_layout()  # otherwise the right y-label is slightly clipped
    plt.grid(color='black', linestyle='-', linewidth=0.5)
    plt.show()


def getPhaseStatusPoint(dataFrame, startTime):

    greenRectangleStartPoint = []
    greenRectangleTime = []
    clearanceRectangleStartPoint = []
    clearanceRectangleTime = []

    currentPhaseStatusStartTime = startTime

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


def getTrajectoryPoint(dataFrame, startTime, approachLength):
    timePoint = []
    distancePoint = []
    for idx, row in dataFrame.loc[:].iterrows():
        # if row['CellStatus'] == 1:
        #     timePoint.append(row['TimeStamp']-startTime)
        #     distancePoint.append(approachLength - row['DistanceToStopBar'])
        if row['CellStatus'] >0 and row['PredictedCellStatus'] > 0.15:
            timePoint.append(row['TimeStamp']-startTime)
            distancePoint.append(approachLength - row['DistanceToStopBar'])

        elif row['CellStatus'] <=0 and row['PredictedCellStatus'] <= 0.05:
            timePoint.append(row['TimeStamp']-startTime)
            distancePoint.append(approachLength - row['DistanceToStopBar'])

    return timePoint, distancePoint


def main():
    # Read the config file into a json object:
    configFile = open("configuration.json", 'r')
    config = (json.load(configFile))
    # Close the config file:
    configFile.close()

    dataFrame = pd.read_csv(config["FileName"])
    approachLength = config["ApproachLength"]
    startTime = dataFrame['TimeStamp'][0]
    timeLength = dataFrame['TimeStamp'].iloc[-1] - dataFrame['TimeStamp'][0]
    print("Start Time is:", startTime)

    timePoint, distancePoint = getTrajectoryPoint(
        dataFrame, startTime, approachLength)

    greenRectangleStartPoint, greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime = getPhaseStatusPoint(
        dataFrame, startTime)

    print("Iteration is done")

    timeSpaceDiagram(timePoint, distancePoint, approachLength, timeLength, greenRectangleStartPoint,
                     greenRectangleTime, clearanceRectangleStartPoint, clearanceRectangleTime)


if __name__ == "__main__":
    main()
