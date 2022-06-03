#pragma once
#include <algorithm>
#include "TrafficSignalPlan.h"
#include "COP.h"

class StageManager
{
private:
    int stageNo{};
    int ringBarrierGroupNo{};
    vector<int> P11{};
    vector<int> P12{};
    vector<int> P13{};
    vector<int> P14{};
    vector<int> P21{};
    vector<int> P22{};
    vector<int> P23{};
    vector<int> P24{};
    vector<int> inactiveSignalGroup{};

    vector<TrafficControllerData::TrafficConrtollerStatus> trafficControllerStatus{};
    vector<COP::plannedSignaBarrierGroup> plannedSignalBarrierGroupList{};
    vector<COP::StagePlan> plannedStages{};

public:
    StageManager(vector<TrafficControllerData::TrafficConrtollerStatus> traffic_Controller_Status, vector<int> inactive_SignalGroup,
                 vector<int> P_11, vector<int> P_12, vector<int> P_13, vector<int> P_14,
                 vector<int> P_21, vector<int> P_22, vector<int> P_23, vector<int> P_24);
    ~StageManager();

    void setStagePlan();
    void processStagePlanPerSignalBarrierGroup(vector<int> signalBarrierGroupRing1, vector<int> signalBarrierGroupRing2);
    void removeInactivePhases();
    vector<COP::StagePlan> getPlannedStages();
};
