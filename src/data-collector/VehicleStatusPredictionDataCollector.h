/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    VehicleStatusPredictionDataCollector.h
    Created by: Debashis Das
    University of Arizona
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:

    1. This script is the header file for VehicleStatusPredictionDataCollector.cpp

*/

#pragma once
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <chrono>
#include "DataPointStructure.h"
#include "BasicVehicle.h"
#include <json/json.h>

using std::cout;
using std::endl;
using std::fixed;
using std::ifstream;
using std::ofstream;
using std::setprecision;
using std::showpoint;
using std::string;
using std::stringstream;
using std::vector;

#define FeetToMeterConversion 0.3048
#define StandardQueueLengthPerVehicle 25.0
#define green 1
#define yellow 2
#define permissive_yellow 3
#define red 4

enum msgType
{
    CurrentState_VehiclePhases = 1,
    VehicleStatus = 2
};

class VehicleStatusPredictionDataCollector
{
private:
    int approachId1{};
    int approachId2{};
    int approachId3{};
    int approachId4{};
    int approach1ThroughSignalGroup{};
    int approach1LeftTurnSignalGroup{};
    int approach2ThroughSignalGroup{};
    int approach2LeftTurnSignalGroup{};
    int approach3ThroughSignalGroup{};
    int approach3LeftTurnSignalGroup{};
    int approach4ThroughSignalGroup{};
    int approach4LeftTurnSignalGroup{};
    int totalNoOfCells{};
    int dataStructurewidth{};
    int dataStructureHeight{};
    int noOfConnectedVehicle{};
    int noOfNonConnectedVehicle{};
    double cellLength{};
    double penetrationRate{};
    double msgSendingTime{};
    bool trainingData{};
    string intersectionName{};

    ofstream logFileApproach1;
    ofstream logFileApproach2;
    ofstream logFileApproach3;
    ofstream logFileApproach4;

    vector<DataPointStructure> DataPointListApproach1{};
    vector<DataPointStructure> DataPointListApproach2{};
    vector<DataPointStructure> DataPointListApproach3{};
    vector<DataPointStructure> DataPointListApproach4{};
    vector<DataPointStructure> InputDataPointListApproach1{};
    vector<DataPointStructure> InputDataPointListApproach2{};
    vector<DataPointStructure> InputDataPointListApproach3{};
    vector<DataPointStructure> InputDataPointListApproach4{};
    vector<DataPointStructure> OutputDataPointListApproach1{};
    vector<DataPointStructure> OutputDataPointListApproach2{};
    vector<DataPointStructure> OutputDataPointListApproach3{};
    vector<DataPointStructure> OutputDataPointListApproach4{};

    vector<int> approach1LaneId{};
    vector<int> approach2LaneId{};
    vector<int> approach3LaneId{};
    vector<int> approach4LaneId{};
    vector<int> approach1SignalGroup{};
    vector<int> approach2SignalGroup{};
    vector<int> approach3SignalGroup{};
    vector<int> approach4SignalGroup{};

public:
    VehicleStatusPredictionDataCollector();
    ~VehicleStatusPredictionDataCollector();

    int getMessageType(string jsonString);
    void readIntersectionInformationConfig();
    void createDataPointStructure(vector<int> signalGroup, vector<int> laneId, int approachId);
    void updatePhaseStatusInDataPointList(vector<DataPointStructure> dataPointList, string jsonString, int approachId, int throughSignalGroup, int leftTurnSignalGroup);
    void processSpatData(string jsonString);
    void fillUpDataPointList(vector<DataPointStructure> InputDataPointList, int approachId, int vehicleId, int vehicleType, double vehicleSpeed, double distanceToStopBar, int laneId, bool connectedVehicleStatus);
    void processVehicleStatusData(string jsonString);
    void createLogFile(ofstream& logFile, int approachId);
    void writeCsvFile(ofstream& logFile, vector<DataPointStructure> InputDataPointList);
    // string getVehicleStatusListRequestMessage();
    // bool checkVehicleStatusListMessageSendingRequirement();
    // vector<int> generateRandomNumber(int noOfVehicle, int noOfInputVehicle);
};
