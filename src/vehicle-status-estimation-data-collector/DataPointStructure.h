#pragma once
// #include<string>
// using std::string;

struct DataPointStructure
{
    int cellNo{};
    int vehicleID{};
    int vehicleType{};
    int signalGroup{};
    int laneId{};
    int approachId{};
    int locationOnMap{};
    int phaseStatus{};
    // bool nearestCVStatus{};
    bool frontCellStatus{};
    double frontCellVehicleSpeed{};
    // double backCVDistanceToStopBar{};
    // double backCVSpeed{};
    double phaseElapsedTime{};
    double phaseUpdateTime{};
    double speed{};
    double heading{};
    double distanceToStopBar{};
    double stoppedDelay{};
    double cellStartPonit{};
    double cellEndPont{};
    bool cellStatus{};

    void reset()
    {
        cellNo = 0;
        vehicleID = 0;
        vehicleType = 0;
        signalGroup = 0;
        laneId = 0;
        approachId = 0;
        locationOnMap = 0;
        phaseStatus = 0;
        // nearestCVStatus = false;
        frontCellStatus = false;
        frontCellVehicleSpeed = 0.0;
        // backCVDistanceToStopBar = 0.0;
        // backCVSpeed = 0.0;
        phaseElapsedTime = 0.0;
        phaseUpdateTime = 0.0;
        speed = -1.0;
        heading = -1.0;
        distanceToStopBar = 0.0;
        stoppedDelay = 0.0;
        cellStartPonit = 0.0;
        cellEndPont = 0.0;
        cellStatus = false;
    }
};