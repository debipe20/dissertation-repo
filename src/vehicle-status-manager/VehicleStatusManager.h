/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    VehicleStatusManager.h
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. This script is the header file for VehicleStatusManager.cpp

*/

#pragma once
#include <iostream>
#include <fstream>
#include <chrono>
#include "VehicleStatus.h"
#include "BasicVehicle.h"
#include <json/json.h>
#include "locAware.h"

#define Vehicle_Timed_Out_Value 10.0

using std::vector;
using std::string;
using std::stringstream;
using std::ifstream;
using std::ofstream;
using std::cout;
using std::endl;

class VehicleStatusManager
{
private:
    string fmap{};
    string intersectionName{};
    uint16_t intersectionId{};
    uint16_t regionalId{};

    LocAware *plocAwareLib;
    vector<VehicleStatus>VehcileStatusList{};

public:
    VehicleStatusManager(string configFilename);
    ~VehicleStatusManager();
    void getVehicleInformationFromMAP(BasicVehicle basicVehicle);
    void manageVehicleStatusList(BasicVehicle basicVehicle);
    bool addVehicleIDInVehicleStatusList(BasicVehicle basicVehicle);
    bool updateVehicleIDInVehicleStatusList(BasicVehicle basicVehicle);
    void deleteTimedOutVehicleIdFromVehicleStatusList();
    string readIntersectionMapConfig(string configFilename);
    string getVehicleStatusList(int requested_ApproachId);
};
