#pragma once
#include <string>

using std::string;

struct VehicleStatus
{
    int vehicleId{};
    string vehicleType{};
    double vehicleLatitude_DecimalDegree{};
    double vehicleLongitude_DecimalDegree{};
    double vehicleElevation_Meter{};
    double vehicleSpeed_MeterPerSecond{};
    double vehicleHeading_Degree{};
    int vehicleLaneId{};
    int vehicleApproachId{};
    int vehicleSignalGroup{};
    double vehicleDistanceFromStopBar{};
    double vehicleStoppedDelay{};
    string vehicleLocationOnMap{};
    double updateTime{};

    void reset()
    {
        vehicleId = 0;
        vehicleType = "";
        vehicleLatitude_DecimalDegree = 0.0;
        vehicleLongitude_DecimalDegree = 0.0;
        vehicleElevation_Meter = 0.0;
        vehicleSpeed_MeterPerSecond = 0.0;
        vehicleHeading_Degree = 0.0;
        vehicleLaneId = 0;
        vehicleApproachId = 0;
        vehicleSignalGroup = 0;
        vehicleDistanceFromStopBar = 0.0;
        vehicleStoppedDelay = 0.0;
        vehicleLocationOnMap = "";
        updateTime = 0.0;
    }
};
