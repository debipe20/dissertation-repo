#pragma once
// #include<string>
// using std::string;

struct DataPointStructure
{
    int vehicleID{};
    int vehicleType{};
    int signalGroup{};
    int laneId{};
    int approachId{};
    int locationOnMap{};
    int phaseStatus{};
    double speed{};
    double heading{};
    double distanceToStopBar{};
    double stoppedDelay{};
    double cellStartPonit{};
    double cellEndPont{};
    // double timeStamp{};
    bool cellStatus{};
    
    void reset()
    {
        vehicleID = 0;
        vehicleType = 0;
        signalGroup = 0;
        laneId = 0;
        approachId = 0;
        locationOnMap = 0;
        phaseStatus = 0;
        speed = 0.0;
        heading = 0.0;
        distanceToStopBar = 0.0;
        stoppedDelay = 0.0;
        cellStartPonit = 0.0;
        cellEndPont = 0.0;
        // timeStamp = 0.0;
        cellStatus = false;
    }
};