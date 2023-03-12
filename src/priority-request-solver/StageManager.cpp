#include "StageManager.h"

StageManager::StageManager(vector<TrafficControllerData::TrafficConrtollerStatus> traffic_Controller_Status, vector<int> inactive_SignalGroup,
                           vector<int> P_11, vector<int> P_12, vector<int> P_13, vector<int> P_14,
                           vector<int> P_21, vector<int> P_22, vector<int> P_23, vector<int> P_24)
{

    trafficControllerStatus = traffic_Controller_Status;
    inactiveSignalGroup = inactive_SignalGroup;
    P11 = P_11;
    P12 = P_12;
    P13 = P_13;
    P14 = P_14;
    P21 = P_21;
    P22 = P_22;
    P23 = P_23;
    P24 = P_24;
    
    removeInactivePhases();
}

/*
    - The following method identifies the required signal group per stage
*/
void StageManager::setStagePlan()
{
    COP::plannedSignaBarrierGroup signalBarrierGroup;
    /*
        - The model is formulated as dual-ring eight-phase controller structure.
        - There could be 4 barriers at most per ring. 11, 12 means ring1 barrier1, ring1 barrier 2 respectively.
    */
    vector<int> signalBarrierGroupRing1List1{11, 12, 13, 14};
    int temporarySignalBarrierGroupNo{};

    plannedStages.clear();
    plannedSignalBarrierGroupList.clear();

    if (find(P11.begin(), P11.end(), trafficControllerStatus[0].startingPhase1) != P11.end())
        temporarySignalBarrierGroupNo = 11;

    else if (find(P12.begin(), P12.end(), trafficControllerStatus[0].startingPhase1) != P12.end())
        temporarySignalBarrierGroupNo = 12;

    else if (find(P13.begin(), P13.end(), trafficControllerStatus[0].startingPhase1) != P13.end())
        temporarySignalBarrierGroupNo = 13;

    else if (find(P14.begin(), P14.end(), trafficControllerStatus[0].startingPhase1) != P14.end())
        temporarySignalBarrierGroupNo = 14;

    // Iterate signalBarrierGroupRing1List1 and identifies the required signal barrier group
    // If signal barrier group No is greater than temporarySignalBarrierGroupNo it will be appened in plannedSignalBarrierGroupList
    for (size_t i = 0; i < signalBarrierGroupRing1List1.size(); i++)
    {
        signalBarrierGroup.reset();

        if (signalBarrierGroupRing1List1.at(i) >= temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 11)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P11;
                signalBarrierGroup.signaBarrierGroup2 = P21;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }

        else if (signalBarrierGroupRing1List1.at(i) >= temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 12)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P12;
                signalBarrierGroup.signaBarrierGroup2 = P22;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }

        else if (signalBarrierGroupRing1List1.at(i) >= temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 13)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P13;
                signalBarrierGroup.signaBarrierGroup2 = P23;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }

        else if (signalBarrierGroupRing1List1.at(i) >= temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 14)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P14;
                signalBarrierGroup.signaBarrierGroup2 = P24;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }
    }

    // Iterate signalBarrierGroupRing1List1 and identifies the required signal barrier group
    // If signal barrier group No is less than temporarySignalBarrierGroupNo it will be appened in plannedSignalBarrierGroupList
    for (size_t i = 0; i < signalBarrierGroupRing1List1.size(); i++)
    {
        signalBarrierGroup.reset();

        if (signalBarrierGroupRing1List1.at(i) < temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 11)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P11;
                signalBarrierGroup.signaBarrierGroup2 = P21;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }

        else if (signalBarrierGroupRing1List1.at(i) < temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 12)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P12;
                signalBarrierGroup.signaBarrierGroup2 = P22;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }

        else if (signalBarrierGroupRing1List1.at(i) < temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 13)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P13;
                signalBarrierGroup.signaBarrierGroup2 = P23;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }

        else if (signalBarrierGroupRing1List1.at(i) < temporarySignalBarrierGroupNo && signalBarrierGroupRing1List1.at(i) == 14)
        {
            {
                signalBarrierGroup.signaBarrierGroup1 = P14;
                signalBarrierGroup.signaBarrierGroup2 = P24;
                plannedSignalBarrierGroupList.push_back(signalBarrierGroup);
            }
        }
    }

    for (size_t i = 0; i < plannedSignalBarrierGroupList.size(); i++)
        processStagePlanPerSignalBarrierGroup(plannedSignalBarrierGroupList[i].signaBarrierGroup1, plannedSignalBarrierGroupList[i].signaBarrierGroup2);

    ringBarrierGroupNo = 0;

    for (size_t i = 0; i < plannedSignalBarrierGroupList.size(); i++)
        processStagePlanPerSignalBarrierGroup(plannedSignalBarrierGroupList[i].signaBarrierGroup1, plannedSignalBarrierGroupList[i].signaBarrierGroup2);
}

/*
    - Method to get the stages based on signal barrier group
*/
void StageManager::processStagePlanPerSignalBarrierGroup(vector<int> signalBarrierGroupRing1, vector<int> signalBarrierGroupRing2)
{
    int indexOfStartingPhase1{};
    int indexOfStartingPhase2{};
    COP::StagePlan stagePlan;
    stagePlan.reset();

    //Remove the served phases for first signal barrier group, based on the statrting phase
    if (stageNo == 0)
    {
        auto it1 = find(signalBarrierGroupRing1.begin(), signalBarrierGroupRing1.end(), trafficControllerStatus[0].startingPhase1);

        if (it1 != signalBarrierGroupRing1.end())
            indexOfStartingPhase1 = static_cast<int>(it1 - signalBarrierGroupRing1.begin());

        if (indexOfStartingPhase1 == 1)
            signalBarrierGroupRing1.erase(signalBarrierGroupRing1.begin());

        auto it2 = find(signalBarrierGroupRing2.begin(), signalBarrierGroupRing2.end(), trafficControllerStatus[0].startingPhase2);

        if (it2 != signalBarrierGroupRing2.end())
            indexOfStartingPhase2 = static_cast<int>(it2 - signalBarrierGroupRing2.begin());

        if (indexOfStartingPhase2 == 1)
            signalBarrierGroupRing2.erase(signalBarrierGroupRing2.begin());
    }

    for (size_t i = 0; i < signalBarrierGroupRing1.size(); i++)
    {
        stagePlan.noOfPhase = static_cast<int>(signalBarrierGroupRing1.size()) + static_cast<int>(signalBarrierGroupRing2.size());

        if (i == 0)
            stagePlan.signalGroup1InRing1 = signalBarrierGroupRing1.at(i);

        else
            stagePlan.signalGroup2InRing1 = signalBarrierGroupRing1.at(i);
    }

    for (size_t i = 0; i < signalBarrierGroupRing2.size(); i++)
    {
        stagePlan.noOfPhase = static_cast<int>(signalBarrierGroupRing1.size()) + static_cast<int>(signalBarrierGroupRing2.size());

        if (i == 0)
            stagePlan.signalGroup1InRing2 = signalBarrierGroupRing2.at(i);

        else
            stagePlan.signalGroup2InRing2 = signalBarrierGroupRing2.at(i);
    }

    if (stagePlan.noOfPhase > 0)
    {
        stageNo++;
        ringBarrierGroupNo++;
        stagePlan.stageNo = stageNo;
        stagePlan.signalBarrierGroupNo = ringBarrierGroupNo;
        plannedStages.push_back(stagePlan);
    }
}

void StageManager::removeInactivePhases()
{
    int temporaryPhase{};

    vector<int> Original_P11 = P11;
    vector<int> Original_P12 = P12;
    vector<int> Original_P13 = P13;
    vector<int> Original_P14 = P14;
    vector<int> Original_P21 = P21;
    vector<int> Original_P22 = P22;
    vector<int> Original_P23 = P23;
    vector<int> Original_P24 = P24;

    for(size_t i = 0; i < inactiveSignalGroup.size(); i++)
    {
        temporaryPhase = inactiveSignalGroup[i];

        if (std::find(P11.begin(), P11.end(), temporaryPhase) != P11.end())
            P11.erase(remove(P11.begin(), P11.end(), temporaryPhase), P11.end());

        else if (std::find(P12.begin(), P12.end(), temporaryPhase) != P12.end())
            P12.erase(remove(P12.begin(), P12.end(), temporaryPhase), P12.end());

        else if (std::find(P13.begin(), P13.end(), temporaryPhase) != P13.end())
            P13.erase(remove(P13.begin(), P13.end(), temporaryPhase), P13.end());

        else if (std::find(P14.begin(), P14.end(), temporaryPhase) != P14.end())
            P14.erase(remove(P14.begin(), P14.end(), temporaryPhase), P14.end());

        else if (std::find(P21.begin(), P21.end(), temporaryPhase) != P21.end())
            P21.erase(remove(P21.begin(), P21.end(), temporaryPhase), P21.end());

        else if (std::find(P22.begin(), P22.end(), temporaryPhase) != P22.end())
            P22.erase(remove(P22.begin(), P22.end(), temporaryPhase), P22.end());

        else if (std::find(P23.begin(), P23.end(), temporaryPhase) != P23.end())
            P23.erase(remove(P23.begin(), P23.end(), temporaryPhase), P23.end());

        else if (std::find(P24.begin(), P24.end(), temporaryPhase) != P24.end())
            P24.erase(remove(P24.begin(), P24.end(), temporaryPhase), P24.end());
    }

    // Added the logic for a scenario when there no signal group on the other ring barrier group 
    if (!P11.empty() && P21.empty())
        P21.push_back(Original_P21.back());

    else if (!P21.empty() && P11.empty())
        P11.push_back(Original_P11.back());

    if (!P12.empty() && P22.empty())
        P22.push_back(Original_P22.back());

    else if (!P22.empty() && P12.empty())
        P12.push_back(Original_P12.back());

    if (!P13.empty() && P23.empty())
        P23.push_back(Original_P23.back());

    else if (!P23.empty() && P13.empty())
        P13.push_back(Original_P13.back());

    if (!P14.empty() && P24.empty())
        P24.push_back(Original_P24.back());

    else if (!P24.empty() && P14.empty())
        P14.push_back(Original_P14.back());

    // To handle a scenario for T-intersection when there are no phase calls on the minor street.
    // TCI fails to translate schedule to SPaT when there are no schedule for conflicting ring-barrier.
    if (!P11.empty() && P12.empty() && P13.empty() && P14.empty())
        P12.push_back(Original_P12.back());

    else if (!P12.empty() && P11.empty() && P13.empty() && P14.empty())
        P11.push_back(Original_P11.back());

    if (!P21.empty() && P22.empty() && P23.empty() && P24.empty())
        P22.push_back(Original_P22.back());

    else if (!P22.empty() && P21.empty() && P23.empty() && P24.empty())
        P21.push_back(Original_P21.back());
}

vector<COP::StagePlan> StageManager::getPlannedStages()
{
    setStagePlan();
    return plannedStages;
}

StageManager::~StageManager()
{
}