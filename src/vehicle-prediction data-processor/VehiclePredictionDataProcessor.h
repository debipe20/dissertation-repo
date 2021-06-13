/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    VehiclePredictionDataProcessor.h
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. This script is the header file for VehiclePredictionDataProcessor.cpp

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
#include "locAware.h"
#include "msgEnum.h"

using std::vector;
using std::string;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;
using std::fixed;
using std::setprecision;
using std::showpoint;

#define FeetToMeterConversion 0.3048
#define StandardQueueLengthPerVehicle 25.0

class VehiclePredictionDataProcessor
{
private:
    string fmap{};
    string intersectionName{};
    uint16_t intersectionId{};
    uint16_t regionalId{};
    ofstream logFile;

    LocAware *plocAwareLib;
    vector<DataPointStructure>DataPointList;

public:
    VehiclePredictionDataProcessor(string configFilename);
    ~VehiclePredictionDataProcessor();
    void getVehicleInformationFromMAP(BasicVehicle basicVehicle);
    int getMessageType(string jsonString);
    string readIntersectionMapConfig(string configFilename);
    void createDataPointStructure();
    void getNoOfCells();

};
