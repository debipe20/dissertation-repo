#pragma once
#include <string>

using std::string;

struct VehicleStatus
{
    int vehicleId{};
    int vehicleType{};
    double vehicleSpeed_MeterPerSecond{};
    double vehicleHeading_Degree{};
    int vehicleLaneId{};
    int vehicleApproachId{};
    int vehicleSignalGroup{};
    double vehicleDistanceFromStopBar{};
    double vehicleStoppedDelay{};
    int vehicleLocationOnMap{};
    bool connected{};
    double updateTime{};
    

    void reset()
    {
        vehicleId = 0;
        vehicleType = 0;
        vehicleSpeed_MeterPerSecond = 0.0;
        vehicleHeading_Degree = 0.0;
        vehicleLaneId = 0;
        vehicleApproachId = 0;
        vehicleSignalGroup = 0;
        vehicleDistanceFromStopBar = 0.0;
        vehicleStoppedDelay = 0.0;
        vehicleLocationOnMap = 0;
        connected = false;
        updateTime = 0.0;
    }
};
