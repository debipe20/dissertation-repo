/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    VehiclePredictionDataCollector.h
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. This script is the header file for VehiclePredictionDataCollector.cpp

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

class VehiclePredictionDataCollector
{
private:
    int approachId{};
    int noOfThroughLanes{};
    int throughLanesLength{};
    int noOfLeftTurnPockets{};
    int leftTurnPocketsLength{};
    int noOfCellsPerThroughLane{};
    int noOfCellsPerLeftTurnPocket{};
    int throughLaneSignalGroup{};
    int leftTurnPocketSignalGroup{};
    int throughLanePhaseStatus{};
    int leftTurnPocketPhaseStatus{};
    double cellLength{};
    double penetrationRate{};
    double msgSendingTime{};
    double msgSendingFrequency{};

    ofstream logFile;

    vector<DataPointStructure> DataPointList{};
    vector<DataPointStructure> InputDataPointList{};
    vector<DataPointStructure> OutputDataPointList{};
    vector<int>leftTurnPocketsId{};
    vector<int>throughLanesId{};

public:
    VehiclePredictionDataCollector();
    ~VehiclePredictionDataCollector();

    int getMessageType(string jsonString);
    void readInterIntersectionInformationConfig();
    void createDataPointStructure();
    void updatePhaseStatusInDataPointList(string jsonString);
    void fillUpDataPointList(string jsonString);
    string getVehicleStatusListRequestMessage();
    bool checkVehicleStatusListMessageSendingRequirement();
};
