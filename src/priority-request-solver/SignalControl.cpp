/*
**********************************************************************************
 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
**********************************************************************************
  SignalControl.cpp
  Created by: Debashis Das
  University of Arizona
  College of Engineering
  This code was developed under the supervision of Professor Larry Head
  in the Systems and Industrial Engineering Department.
  Revision History:
  1.
  2. This script contains method to develop optimal schedule in a JSON formatted message.
*/

#include "SignalControl.h"

SignalControl::SignalControl(string objective_Function, int time_Horizon,
                             vector<TrafficControllerData::TrafficSignalPlan> signalPlan, vector<TrafficControllerData::TrafficConrtollerStatus> signalStatus, vector<int> phase_Call_List,
                             vector<ArrivalTable> vehicle_Arrival_Table, vector<int> arrival_PhaseCall_List,
                             vector<int> P_11, vector<int> P_12, vector<int> P_13, vector<int> P_14,
                             vector<int> P_21, vector<int> P_22, vector<int> P_23, vector<int> P_24)
{
    objectiveFunction = objective_Function;
    timeHorizon = time_Horizon;

    if (!signalPlan.empty())
        trafficSignalPlan = signalPlan;

    if (!signalStatus.empty())
        trafficControllerStatus = signalStatus;

    if (!vehicle_Arrival_Table.empty())
        vehicleArrivalTable = vehicle_Arrival_Table;

    if (!arrival_PhaseCall_List.empty())
        arrivalPhaseCallList = arrival_PhaseCall_List;

    phaseCallList = phase_Call_List;

    P11 = P_11;
    P12 = P_12;
    P13 = P_13;
    P14 = P_14;
    P21 = P_21;
    P22 = P_22;
    P23 = P_23;
    P24 = P_24;

    timeHorizon = 120;
}

/*
    - A signal group will be active if gmin for that phase has positve value and there is a phase call for that phase
*/
void SignalControl::getActiveSignalGroup()
{
    int temporaryPhase{};
    activeSignalGroup.clear();
    inactiveSignalGroup.clear();

    for (size_t i = 0; i < trafficSignalPlan.size(); i++)
    {
        if (trafficSignalPlan[i].minGreen > 0)
            activeSignalGroup.push_back(trafficSignalPlan[i].phaseNumber);
    }

    if (find(phaseCallList.begin(), phaseCallList.end(), trafficControllerStatus[0].startingPhase1) == phaseCallList.end())
        phaseCallList.push_back(trafficControllerStatus[0].startingPhase1);

    if (find(phaseCallList.begin(), phaseCallList.end(), trafficControllerStatus[0].startingPhase2) == phaseCallList.end())
        phaseCallList.push_back(trafficControllerStatus[0].startingPhase2);

    for (size_t i = 0; i < arrivalPhaseCallList.size(); i++)
    {
        if (find(phaseCallList.begin(), phaseCallList.end(), arrivalPhaseCallList[i]) == phaseCallList.end())
            phaseCallList.push_back(arrivalPhaseCallList[i]);
    }

    for (size_t i = 0; i < activeSignalGroup.size(); i++)
    {
        temporaryPhase = activeSignalGroup[i];

        if (std::find(phaseCallList.begin(), phaseCallList.end(), temporaryPhase) == phaseCallList.end())
        {
            activeSignalGroup.erase(remove(activeSignalGroup.begin(), activeSignalGroup.end(), temporaryPhase), activeSignalGroup.end());
            inactiveSignalGroup.push_back(temporaryPhase);
            i--;
        }
    }
}

/*
    - The following method compute gmin, clearance interval and minimum barrier length per ring for each stage (ring-barrier group)
    - The methods considers the elapsed green green time and init time for the first stage while calculating the minimum barrier length
*/
int SignalControl::getMinimumBarrierLength(int stageIndex)
{
    int minimumBarrierGrouplengthInRing1{};
    int minimumBarrierGrouplengthInRing2{};
    int currentStageMinimumBarrierLength{};
    vector<int> minimumBarrierLengthList{};

    gminSignalGroup1InRing1 = 0;
    gminSignalGroup2InRing1 = 0;
    gminSignalGroup1InRing2 = 0;
    gminSignalGroup2InRing2 = 0;
    clearanceIntervalSignalGroup1InRing1 = 0;
    clearanceIntervalSignalGroup2InRing1 = 0;
    clearanceIntervalSignalGroup1InRing2 = 0;
    clearanceIntervalSignalGroup2InRing2 = 0;

    if (plannedStages[stageIndex].signalGroup1InRing1 > 0)
    {
        vector<TrafficControllerData::TrafficSignalPlan>::iterator findSignalGroup1InRing1 =
            std::find_if(std::begin(trafficSignalPlan), std::end(trafficSignalPlan), [&](TrafficControllerData::TrafficSignalPlan const &p)
                         { return p.phaseNumber == plannedStages[stageIndex].signalGroup1InRing1; });

        gminSignalGroup1InRing1 = static_cast<int>(findSignalGroup1InRing1->minGreen);
        clearanceIntervalSignalGroup1InRing1 = static_cast<int>(findSignalGroup1InRing1->yellowChange) + static_cast<int>(findSignalGroup1InRing1->redClear);
    }

    if (plannedStages[stageIndex].signalGroup2InRing1 > 0)
    {
        vector<TrafficControllerData::TrafficSignalPlan>::iterator findSignalGroup2InRing1 = std::find_if(std::begin(trafficSignalPlan), std::end(trafficSignalPlan), [&](TrafficControllerData::TrafficSignalPlan const &p)
                                                                                                          { return p.phaseNumber == plannedStages[stageIndex].signalGroup2InRing1; });

        gminSignalGroup2InRing1 = static_cast<int>(findSignalGroup2InRing1->minGreen);
        clearanceIntervalSignalGroup2InRing1 = static_cast<int>(findSignalGroup2InRing1->yellowChange) + static_cast<int>(findSignalGroup2InRing1->redClear);
    }

    if (plannedStages[stageIndex].signalGroup1InRing2 > 0)
    {
        vector<TrafficControllerData::TrafficSignalPlan>::iterator findSignalGroup1InRing2 = std::find_if(std::begin(trafficSignalPlan), std::end(trafficSignalPlan), [&](TrafficControllerData::TrafficSignalPlan const &p)
                                                                                                          { return p.phaseNumber == plannedStages[stageIndex].signalGroup1InRing2; });

        gminSignalGroup1InRing2 = static_cast<int>(findSignalGroup1InRing2->minGreen);
        clearanceIntervalSignalGroup1InRing2 = static_cast<int>(findSignalGroup1InRing2->yellowChange) + static_cast<int>(findSignalGroup1InRing2->redClear);
    }

    if (plannedStages[stageIndex].signalGroup2InRing2 > 0)
    {
        vector<TrafficControllerData::TrafficSignalPlan>::iterator findSignalGroup2InRing2 = std::find_if(std::begin(trafficSignalPlan), std::end(trafficSignalPlan), [&](TrafficControllerData::TrafficSignalPlan const &p)
                                                                                                          { return p.phaseNumber == plannedStages[stageIndex].signalGroup2InRing2; });

        gminSignalGroup2InRing2 = static_cast<int>(findSignalGroup2InRing2->minGreen);
        clearanceIntervalSignalGroup2InRing2 = static_cast<int>(findSignalGroup2InRing2->yellowChange) + static_cast<int>(findSignalGroup2InRing2->redClear);
    }

    if (stageIndex == 0)
    {
        if ((gminSignalGroup1InRing1 + gminSignalGroup2InRing1 - static_cast<int>(trafficControllerStatus[0].elapsedGreen1)) > 0)
            minimumBarrierGrouplengthInRing1 = gminSignalGroup1InRing1 + clearanceIntervalSignalGroup1InRing1 +
                                               gminSignalGroup2InRing1 + clearanceIntervalSignalGroup2InRing1 -
                                               static_cast<int>(trafficControllerStatus[0].elapsedGreen1) +
                                               static_cast<int>(trafficControllerStatus[0].initPhase1);

        else
            minimumBarrierGrouplengthInRing1 = clearanceIntervalSignalGroup1InRing1 + clearanceIntervalSignalGroup2InRing1;

        if ((gminSignalGroup1InRing2 + gminSignalGroup2InRing2 - static_cast<int>(trafficControllerStatus[0].elapsedGreen2)) > 0)
            minimumBarrierGrouplengthInRing2 = gminSignalGroup1InRing2 + clearanceIntervalSignalGroup1InRing2 +
                                               gminSignalGroup2InRing2 + clearanceIntervalSignalGroup2InRing2 -
                                               static_cast<int>(trafficControllerStatus[0].elapsedGreen2) +
                                               static_cast<int>(trafficControllerStatus[0].initPhase2);

        else
            minimumBarrierGrouplengthInRing2 = clearanceIntervalSignalGroup1InRing2 + clearanceIntervalSignalGroup2InRing2;
    }

    else
    {
        minimumBarrierGrouplengthInRing1 = gminSignalGroup1InRing1 + clearanceIntervalSignalGroup1InRing1 + gminSignalGroup2InRing1 + clearanceIntervalSignalGroup2InRing1;
        minimumBarrierGrouplengthInRing2 = gminSignalGroup1InRing2 + clearanceIntervalSignalGroup1InRing2 + gminSignalGroup2InRing2 + clearanceIntervalSignalGroup2InRing2;
    }

    minimumBarrierLengthList.push_back(minimumBarrierGrouplengthInRing1);
    minimumBarrierLengthList.push_back(minimumBarrierGrouplengthInRing2);
    currentStageMinimumBarrierLength = *max_element(minimumBarrierLengthList.begin(), minimumBarrierLengthList.end());

    return currentStageMinimumBarrierLength;
}

/*
    - Following method performs the forward recurrsion of the DP. It computes performance measurement (delay) for each stage
        - The method calls getActiveSignalGroup() method to compute active signal groupu list
        - The method creats an instance for StageManager class to compute planned stages.
        - The method identifies inactive signal group each stage
        - The method follows dual-ring eight-phase controller logic and performs recurssion logic for each stage
        - The method checks for stopping criteria while performing recurssion logic
        - At final step, method calls signalControlAlgorithmBackwardRecursion() method to execute backward recurssion
*/
void SignalControl::signalControlAlgorithmForwardRecursion()
{
    int timeStep{};
    int allocatedTimeToPreviousStage{};
    int allocatedTimeToCurrentStage{};
    int currentStageMinimumBarrierLength{};
    int sumOfMinimumTimeRequirementForPreviousStage{};
    double previousStageValueFunction{};
    double temporaryActivePhasesPerformancemeasurementValue{};
    double temporaryTotalPerformanceMeasurement{};
    double previousPerformanceMeasurement{};
    double minPerformanceMeasurement{};
    vector<int> signalGroup1AllocatedTimeStepList{};
    vector<int> inactiveSignalgroupInCurrentStage{};
    vector<int> previousStageTimeStepList{};
    vector<int> temporaryAllocatedTimeToCurrentStageSignalGroup1InRing1List{};
    vector<int> temporaryAllocatedTimeToCurrentStageSignalGroup2InRing1List{};
    vector<int> temporaryAllocatedTimeToCurrentStageSignalGroup1InRing2List{};
    vector<int> temporaryAllocatedTimeToCurrentStageSignalGroup2InRing2List{};
    vector<int> temporaryAllocatedTimeToPreviousStageList{};
    vector<int> temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing1List{};
    vector<int> temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing1List{};
    vector<int> temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing2List{};
    vector<int> temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing2List{};
    vector<double> temporaryActivePhasesPerformancemeasurementList{};
    vector<double> temporaryInactivePhasesPerformancemeasurementList{};
    vector<double> temporaryPreviousStagePerformanceMeasurementList{};
    vector<double> temporaryTotalPerformmanceMeasurementList{};

    COP::PerformanceMeasurement performance;

    plannedStages.clear();
    performanceMeasurementTable.clear();
    optimalSignalTiming.clear();
    previousStageMinimumBarrierLengthList.clear();

    getActiveSignalGroup();
    StageManager stageManger(trafficControllerStatus, inactiveSignalGroup, P11, P12, P13, P14, P21, P22, P23, P24);
    plannedStages = stageManger.getPlannedStages();

    for (size_t stageIndex = 0; stageIndex < plannedStages.size(); stageIndex++)
    {
        inactiveSignalgroupInCurrentStage = activeSignalGroup;
        currentStageMinimumBarrierLength = getMinimumBarrierLength(static_cast<int>(stageIndex));

        inactiveSignalgroupInCurrentStage.erase(remove(inactiveSignalgroupInCurrentStage.begin(), inactiveSignalgroupInCurrentStage.end(), plannedStages[stageIndex].signalGroup1InRing1), inactiveSignalgroupInCurrentStage.end());
        inactiveSignalgroupInCurrentStage.erase(remove(inactiveSignalgroupInCurrentStage.begin(), inactiveSignalgroupInCurrentStage.end(), plannedStages[stageIndex].signalGroup2InRing1), inactiveSignalgroupInCurrentStage.end());
        inactiveSignalgroupInCurrentStage.erase(remove(inactiveSignalgroupInCurrentStage.begin(), inactiveSignalgroupInCurrentStage.end(), plannedStages[stageIndex].signalGroup1InRing2), inactiveSignalgroupInCurrentStage.end());
        inactiveSignalgroupInCurrentStage.erase(remove(inactiveSignalgroupInCurrentStage.begin(), inactiveSignalgroupInCurrentStage.end(), plannedStages[stageIndex].signalGroup2InRing2), inactiveSignalgroupInCurrentStage.end());

        if (plannedStages[stageIndex].stageNo == 1)
        {
            for (timeStep = 0; timeStep <= timeHorizon; timeStep++)
            {
                performance.reset();
                allocatedTimeToCurrentStage = timeStep;
                allocatedTimeToPreviousStage = 0;

                // Following logic ensures a phase is served for minimum green time
                if (timeStep < currentStageMinimumBarrierLength)
                    continue;

                else
                {
                    currentStageRecurssion(static_cast<int>(stageIndex), timeStep, allocatedTimeToCurrentStage, currentStageMinimumBarrierLength, allocatedTimeToPreviousStage, inactiveSignalgroupInCurrentStage, Ring1);

                    performance.stageNo = plannedStages[stageIndex].stageNo;
                    performance.timeStep = timeStep;
                    performance.signalGroup1InRing1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].signalGroup1;
                    performance.signalGroup2InRing1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].signalGroup2;
                    performance.allocatedTimeStepSignalGroup1Ring1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup1;
                    performance.allocatedTimeStepSignalGroup2Ring1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup2;
                    performance.allocatedGreenTimeSignalGroup1Ring1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup1;
                    performance.allocatedGreenTimeSignalGroup2Ring1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup2;
                    performance.clearanceIntervalSignalGroup1Ring1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].clearanceIntervalSignalGroup1;
                    performance.clearanceIntervalSignalGroup2Ring1 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].clearanceIntervalSignalGroup2;
                    performance.activePhasesPerformancemeasurement = activePhasesPerformanceMeasurementValue;

                    currentStageRecurssion(static_cast<int>(stageIndex), timeStep, allocatedTimeToCurrentStage, currentStageMinimumBarrierLength, allocatedTimeToPreviousStage, inactiveSignalgroupInCurrentStage, Ring2);

                    performance.signalGroup1InRing2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].signalGroup1;
                    performance.signalGroup2InRing2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].signalGroup2;
                    performance.allocatedTimeStepSignalGroup1Ring2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup1;
                    performance.allocatedTimeStepSignalGroup2Ring2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup2;
                    performance.allocatedGreenTimeSignalGroup1Ring2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup1;
                    performance.allocatedGreenTimeSignalGroup2Ring2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup2;
                    performance.clearanceIntervalSignalGroup1Ring2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].clearanceIntervalSignalGroup1;
                    performance.clearanceIntervalSignalGroup2Ring2 = availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].clearanceIntervalSignalGroup2;
                    performance.activePhasesPerformancemeasurement = performance.activePhasesPerformancemeasurement + activePhasesPerformanceMeasurementValue;
                    performance.inactivePhasesPerformancemeasurement = inactivePhasesPerformanceMeasurementValue;
                    performance.totalPerformmanceMeasurement = performance.activePhasesPerformancemeasurement + performance.inactivePhasesPerformancemeasurement;

                    performanceMeasurementTable.push_back(performance);
                }
            }
            previousStageMinimumBarrierLengthList.push_back(currentStageMinimumBarrierLength);
        }

        else
        {
            for (timeStep = 0; timeStep <= timeHorizon; timeStep++)
            {
                sumOfMinimumTimeRequirementForPreviousStage = 0;

                for (size_t i = 0; i < previousStageMinimumBarrierLengthList.size(); i++)
                    sumOfMinimumTimeRequirementForPreviousStage = sumOfMinimumTimeRequirementForPreviousStage + previousStageMinimumBarrierLengthList[i];

                if (timeStep < (currentStageMinimumBarrierLength + sumOfMinimumTimeRequirementForPreviousStage))
                    continue;

                else
                {
                    temporaryAllocatedTimeToCurrentStageSignalGroup1InRing1List.clear();
                    temporaryAllocatedTimeToCurrentStageSignalGroup2InRing1List.clear();
                    temporaryAllocatedTimeToCurrentStageSignalGroup1InRing2List.clear();
                    temporaryAllocatedTimeToCurrentStageSignalGroup2InRing2List.clear();
                    temporaryAllocatedTimeToPreviousStageList.clear();
                    temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing1List.clear();
                    temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing1List.clear();
                    temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing2List.clear();
                    temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing2List.clear();
                    temporaryActivePhasesPerformancemeasurementList.clear();
                    temporaryInactivePhasesPerformancemeasurementList.clear();
                    temporaryPreviousStagePerformanceMeasurementList.clear();
                    temporaryTotalPerformmanceMeasurementList.clear();

                    previousStageTimeStepList = getPreviousStageTimeStep(timeStep, currentStageMinimumBarrierLength);
                    temporaryTotalPerformanceMeasurement = 10000;
                    for (size_t i = 0; i < previousStageTimeStepList.size(); i++)
                    {
                        previousPerformanceMeasurement = temporaryTotalPerformanceMeasurement;
                        allocatedTimeToPreviousStage = previousStageTimeStepList[i];
                        allocatedTimeToCurrentStage = timeStep - allocatedTimeToPreviousStage;

                        previousStageValueFunction = getCummulativeValueFunction(plannedStages[stageIndex].stageNo, allocatedTimeToPreviousStage);

                        currentStageRecurssion(static_cast<int>(stageIndex), timeStep, allocatedTimeToCurrentStage, currentStageMinimumBarrierLength, allocatedTimeToPreviousStage, inactiveSignalgroupInCurrentStage, Ring1);
                        temporaryActivePhasesPerformancemeasurementValue = activePhasesPerformanceMeasurementValue;

                        temporaryAllocatedTimeToCurrentStageSignalGroup1InRing1List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup1);
                        temporaryAllocatedTimeToCurrentStageSignalGroup2InRing1List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup2);
                        temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing1List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup1);
                        temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing1List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup2);

                        currentStageRecurssion(static_cast<int>(stageIndex), timeStep, allocatedTimeToCurrentStage, currentStageMinimumBarrierLength, allocatedTimeToPreviousStage, inactiveSignalgroupInCurrentStage, Ring2);
                        temporaryActivePhasesPerformancemeasurementValue = temporaryActivePhasesPerformancemeasurementValue + activePhasesPerformanceMeasurementValue;

                        temporaryAllocatedTimeToCurrentStageSignalGroup1InRing2List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup1);
                        temporaryAllocatedTimeToCurrentStageSignalGroup2InRing2List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedTimeStepSignalGroup2);
                        temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing2List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup1);
                        temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing2List.push_back(availableOptionListForForwardStage[indexOfMinPerformanceMeasurement].allocatedGreenTimeSignalGroup2);
                        temporaryAllocatedTimeToPreviousStageList.push_back(allocatedTimeToPreviousStage);
                        temporaryActivePhasesPerformancemeasurementList.push_back(temporaryActivePhasesPerformancemeasurementValue);
                        temporaryInactivePhasesPerformancemeasurementList.push_back(inactivePhasesPerformanceMeasurementValue);
                        temporaryPreviousStagePerformanceMeasurementList.push_back(previousStageValueFunction);
                        temporaryTotalPerformanceMeasurement = temporaryActivePhasesPerformancemeasurementValue + inactivePhasesPerformanceMeasurementValue + previousStageValueFunction;

                        temporaryTotalPerformmanceMeasurementList.push_back(temporaryTotalPerformanceMeasurement);

                        if (checkStoppingCrietria(previousPerformanceMeasurement, temporaryTotalPerformanceMeasurement))
                            break;
                    }

                    minPerformanceMeasurement = *min_element(temporaryTotalPerformmanceMeasurementList.begin(), temporaryTotalPerformmanceMeasurementList.end());
                    auto it = find(temporaryTotalPerformmanceMeasurementList.begin(), temporaryTotalPerformmanceMeasurementList.end(), minPerformanceMeasurement);

                    if (it != temporaryTotalPerformmanceMeasurementList.end())
                        indexOfMinPerformanceMeasurement = static_cast<int>(it - temporaryTotalPerformmanceMeasurementList.begin());

                    performance.stageNo = plannedStages[stageIndex].stageNo;
                    performance.timeStep = timeStep;
                    performance.signalGroup1InRing1 = plannedStages[stageIndex].signalGroup1InRing1;
                    performance.signalGroup2InRing1 = plannedStages[stageIndex].signalGroup2InRing1;
                    performance.allocatedTimeStepSignalGroup1Ring1 = temporaryAllocatedTimeToCurrentStageSignalGroup1InRing1List.at(indexOfMinPerformanceMeasurement);
                    performance.allocatedTimeStepSignalGroup2Ring1 = temporaryAllocatedTimeToCurrentStageSignalGroup2InRing1List.at(indexOfMinPerformanceMeasurement);
                    performance.allocatedGreenTimeSignalGroup1Ring1 = temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing1List.at(indexOfMinPerformanceMeasurement);
                    performance.allocatedGreenTimeSignalGroup2Ring1 = temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing1List.at(indexOfMinPerformanceMeasurement);
                    performance.clearanceIntervalSignalGroup1Ring1 = clearanceIntervalSignalGroup1InRing1;
                    performance.clearanceIntervalSignalGroup2Ring1 = clearanceIntervalSignalGroup2InRing1;
                    performance.signalGroup1InRing2 = plannedStages[stageIndex].signalGroup1InRing2;
                    performance.signalGroup2InRing2 = plannedStages[stageIndex].signalGroup2InRing2;
                    performance.allocatedTimeStepSignalGroup1Ring2 = temporaryAllocatedTimeToCurrentStageSignalGroup1InRing2List.at(indexOfMinPerformanceMeasurement);
                    performance.allocatedTimeStepSignalGroup2Ring2 = temporaryAllocatedTimeToCurrentStageSignalGroup2InRing2List.at(indexOfMinPerformanceMeasurement);
                    performance.allocatedGreenTimeSignalGroup1Ring2 = temporaryAllocatedGreenTimeToCurrentStageSignalGroup1InRing2List.at(indexOfMinPerformanceMeasurement);
                    performance.allocatedGreenTimeSignalGroup2Ring2 = temporaryAllocatedGreenTimeToCurrentStageSignalGroup2InRing2List.at(indexOfMinPerformanceMeasurement);
                    performance.clearanceIntervalSignalGroup1Ring2 = clearanceIntervalSignalGroup1InRing2;
                    performance.clearanceIntervalSignalGroup2Ring2 = clearanceIntervalSignalGroup2InRing2;
                    performance.allocatedTimeToPreviousStage = temporaryAllocatedTimeToPreviousStageList.at(indexOfMinPerformanceMeasurement);
                    performance.activePhasesPerformancemeasurement = temporaryActivePhasesPerformancemeasurementList.at(indexOfMinPerformanceMeasurement);
                    performance.inactivePhasesPerformancemeasurement = temporaryInactivePhasesPerformancemeasurementList.at(indexOfMinPerformanceMeasurement);
                    performance.previousStagePerformanceMeasurement = temporaryPreviousStagePerformanceMeasurementList.at(indexOfMinPerformanceMeasurement);
                    performance.totalPerformmanceMeasurement = temporaryTotalPerformmanceMeasurementList.at(indexOfMinPerformanceMeasurement);

                    performanceMeasurementTable.push_back(performance);
                }
            }
            previousStageMinimumBarrierLengthList.push_back(currentStageMinimumBarrierLength);
        }
    }

    // logData();
    signalControlAlgorithmBackwardRecursion();
}

/*
    - Following method performs the backward recurrsion of the DP. It computes optimal policy based on the performance table computes in forward recurssion
    - The recurssion starts with the final stage.
    - At each stage it computes the ploicy that results minimum delay and corresponding allocted time to the previous stage
*/
void SignalControl::signalControlAlgorithmBackwardRecursion()
{
    int currentStage{};
    int allocatedTimeToPreviousStage{};
    double minPerformanceMeasurement{};
    vector<double> finalStagePerformanceMeasurement{};
    COP::OptimalPlan optimalPlan;
    optimalPlan.reset();

    currentStage = performanceMeasurementTable[performanceMeasurementTable.size() - 1].stageNo;

    // Append all the possible perfomance measurement value in a list for the final stage
    for (int i = static_cast<int>(performanceMeasurementTable.size() - 1); i >= 0; --i)
    {
        if (performanceMeasurementTable[i].stageNo == currentStage)
            finalStagePerformanceMeasurement.insert(finalStagePerformanceMeasurement.begin(), performanceMeasurementTable[i].totalPerformmanceMeasurement);
    }

    minPerformanceMeasurement = *min_element(finalStagePerformanceMeasurement.begin(), finalStagePerformanceMeasurement.end());

    // Append optimal policy for the final stage and calculates corresponding allocted time to the previous stage
    for (size_t i = 0; i < performanceMeasurementTable.size(); i++)
    {
        if (performanceMeasurementTable[i].stageNo == currentStage && performanceMeasurementTable[i].totalPerformmanceMeasurement == minPerformanceMeasurement)
        {
            optimalPlan.stageNo = performanceMeasurementTable[i].stageNo;
            optimalPlan.timeStep = performanceMeasurementTable[i].timeStep;
            optimalPlan.signalGroup1InRing1 = performanceMeasurementTable[i].signalGroup1InRing1;
            optimalPlan.signalGroup2InRing1 = performanceMeasurementTable[i].signalGroup2InRing1;
            optimalPlan.signalGroup1InRing2 = performanceMeasurementTable[i].signalGroup1InRing2;
            optimalPlan.signalGroup2InRing2 = performanceMeasurementTable[i].signalGroup2InRing2;
            optimalPlan.allocatedGreenTimeSignalGroup1Ring1 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup1Ring1;
            optimalPlan.allocatedGreenTimeSignalGroup2Ring1 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup2Ring1;
            optimalPlan.allocatedGreenTimeSignalGroup1Ring2 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup1Ring2;
            optimalPlan.allocatedGreenTimeSignalGroup2Ring2 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup2Ring2;
            optimalPlan.clearanceIntervalSignalGroup1Ring1 = performanceMeasurementTable[i].clearanceIntervalSignalGroup1Ring1;
            optimalPlan.clearanceIntervalSignalGroup2Ring1 = performanceMeasurementTable[i].clearanceIntervalSignalGroup2Ring1;
            optimalPlan.clearanceIntervalSignalGroup1Ring2 = performanceMeasurementTable[i].clearanceIntervalSignalGroup1Ring2;
            optimalPlan.clearanceIntervalSignalGroup2Ring2 = performanceMeasurementTable[i].clearanceIntervalSignalGroup2Ring2;
            optimalPlan.optimalPerformmanceMeasurement = performanceMeasurementTable[i].totalPerformmanceMeasurement;

            optimalSignalTiming.push_back(optimalPlan);
            allocatedTimeToPreviousStage = performanceMeasurementTable[i].allocatedTimeToPreviousStage;
            currentStage--;
            break;
        }
    }

    // Performs recurssion for the rest of the stages and obtains optimal policy
    for (int i = static_cast<int>(performanceMeasurementTable.size() - 1); i >= 0; --i)
    {
        if (performanceMeasurementTable[i].stageNo == currentStage && performanceMeasurementTable[i].timeStep == allocatedTimeToPreviousStage)
        {
            optimalPlan.reset();
            optimalPlan.stageNo = performanceMeasurementTable[i].stageNo;
            optimalPlan.timeStep = performanceMeasurementTable[i].timeStep;
            optimalPlan.signalGroup1InRing1 = performanceMeasurementTable[i].signalGroup1InRing1;
            optimalPlan.signalGroup2InRing1 = performanceMeasurementTable[i].signalGroup2InRing1;
            optimalPlan.signalGroup1InRing2 = performanceMeasurementTable[i].signalGroup1InRing2;
            optimalPlan.signalGroup2InRing2 = performanceMeasurementTable[i].signalGroup2InRing2;
            optimalPlan.allocatedGreenTimeSignalGroup1Ring1 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup1Ring1;
            optimalPlan.allocatedGreenTimeSignalGroup2Ring1 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup2Ring1;
            optimalPlan.allocatedGreenTimeSignalGroup1Ring2 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup1Ring2;
            optimalPlan.allocatedGreenTimeSignalGroup2Ring2 = performanceMeasurementTable[i].allocatedGreenTimeSignalGroup2Ring2;
            optimalPlan.clearanceIntervalSignalGroup1Ring1 = performanceMeasurementTable[i].clearanceIntervalSignalGroup1Ring1;
            optimalPlan.clearanceIntervalSignalGroup2Ring1 = performanceMeasurementTable[i].clearanceIntervalSignalGroup2Ring1;
            optimalPlan.clearanceIntervalSignalGroup1Ring2 = performanceMeasurementTable[i].clearanceIntervalSignalGroup1Ring2;
            optimalPlan.clearanceIntervalSignalGroup2Ring2 = performanceMeasurementTable[i].clearanceIntervalSignalGroup2Ring2;
            optimalPlan.optimalPerformmanceMeasurement = performanceMeasurementTable[i].totalPerformmanceMeasurement;

            optimalSignalTiming.insert(optimalSignalTiming.begin(), optimalPlan);
            allocatedTimeToPreviousStage = performanceMeasurementTable[i].allocatedTimeToPreviousStage;
            currentStage--;
        }

        if (currentStage < 1)
            break;
    }

    printOptimalPan();
}

/*
    - Method to perform recurssion logic for signalControlAlgorithmForwardRecursion() method
*/
void SignalControl::currentStageRecurssion(int stageIndex, int timeStep, int allocatedTimeToCurrentStage, int currentStageMinimumBarrierLength, int allocatedTimeToPreviousStage, vector<int> inactiveSignalgroupInCurrentStage, int ringNo)
{
    vector<int> signalGroup1AllocatedTimeStepList{};
    COP::AvailableOptionForForwardStage availableOption;
    availableOptionListForForwardStage.clear();
    availableOption.reset();

    if (ringNo == Ring1)
    {
        if (plannedStages[stageIndex].signalGroup1InRing1 == 0)
        {
            // If signalGroup1InRing1 value is zero for a stage all the allocated time to current stage will be assigned to signalGroup2InRing1
            if (stageIndex == 0)
                allocatedTimeToCurrentStageSignalGroup2InRing1 = allocatedTimeToCurrentStage - static_cast<int>(trafficControllerStatus[0].initPhase1);

            else
                allocatedTimeToCurrentStageSignalGroup2InRing1 = allocatedTimeToCurrentStage;

            allocatedGreenTimeToCurrentStageSignalGroup2InRing1 = allocatedTimeToCurrentStageSignalGroup2InRing1 - clearanceIntervalSignalGroup2InRing1;

            availableOption.signalGroup1 = 0;
            availableOption.allocatedTimeStepSignalGroup1 = 0;
            availableOption.allocatedGreenTimeSignalGroup1 = 0;
            availableOption.minimumGreenTimeSignalGroup1 = 0;
            availableOption.clearanceIntervalSignalGroup1 = 0;
            availableOption.signalGroup2 = plannedStages[stageIndex].signalGroup2InRing1;
            availableOption.allocatedTimeStepSignalGroup2 = allocatedTimeToCurrentStageSignalGroup2InRing1;
            availableOption.allocatedGreenTimeSignalGroup2 = allocatedGreenTimeToCurrentStageSignalGroup2InRing1;
            availableOption.minimumGreenTimeSignalGroup2 = gminSignalGroup2InRing1;
            availableOption.clearanceIntervalSignalGroup2 = clearanceIntervalSignalGroup2InRing1;

            availableOptionListForForwardStage.push_back(availableOption);
        }

        else if (plannedStages[stageIndex].signalGroup2InRing1 == 0)
        {
            // If signalGroup2InRing1 value is zero for a stage all the allocated time to current stage will be assigned to signalGroup1InRing1
            if (stageIndex == 0)
                allocatedTimeToCurrentStageSignalGroup1InRing1 = allocatedTimeToCurrentStage - static_cast<int>(trafficControllerStatus[0].initPhase1);

            else
                allocatedTimeToCurrentStageSignalGroup1InRing1 = allocatedTimeToCurrentStage;

            allocatedGreenTimeToCurrentStageSignalGroup1InRing1 = allocatedTimeToCurrentStageSignalGroup1InRing1 - clearanceIntervalSignalGroup1InRing1;

            availableOption.signalGroup1 = plannedStages[stageIndex].signalGroup1InRing1;
            availableOption.allocatedTimeStepSignalGroup1 = allocatedTimeToCurrentStageSignalGroup1InRing1;
            availableOption.allocatedGreenTimeSignalGroup1 = allocatedGreenTimeToCurrentStageSignalGroup1InRing1;
            availableOption.minimumGreenTimeSignalGroup1 = gminSignalGroup1InRing1;
            availableOption.clearanceIntervalSignalGroup1 = clearanceIntervalSignalGroup1InRing1;
            availableOption.signalGroup2 = 0;
            availableOption.allocatedTimeStepSignalGroup2 = 0;
            availableOption.allocatedGreenTimeSignalGroup2 = 0;
            availableOption.minimumGreenTimeSignalGroup2 = 0;
            availableOption.clearanceIntervalSignalGroup2 = 0;

            availableOptionListForForwardStage.push_back(availableOption);
        }

        else
        {
            signalGroup1AllocatedTimeStepList = getAllocatedTimeStepListToSignalGroup1(allocatedTimeToCurrentStage, currentStageMinimumBarrierLength, gminSignalGroup2InRing1, clearanceIntervalSignalGroup2InRing1);

            for (size_t i = 0; i < signalGroup1AllocatedTimeStepList.size(); i++)
            {
                availableOption.reset();

                if (stageIndex == 0)
                {
                    allocatedTimeToCurrentStageSignalGroup1InRing1 = signalGroup1AllocatedTimeStepList.at(i) - static_cast<int>(trafficControllerStatus[0].initPhase1);
                    allocatedTimeToCurrentStageSignalGroup2InRing1 = allocatedTimeToCurrentStage - allocatedTimeToCurrentStageSignalGroup1InRing1 - static_cast<int>(trafficControllerStatus[0].initPhase1);
                }

                else
                {
                    allocatedTimeToCurrentStageSignalGroup1InRing1 = signalGroup1AllocatedTimeStepList.at(i);
                    allocatedTimeToCurrentStageSignalGroup2InRing1 = allocatedTimeToCurrentStage - allocatedTimeToCurrentStageSignalGroup1InRing1;
                }

                allocatedGreenTimeToCurrentStageSignalGroup1InRing1 = allocatedTimeToCurrentStageSignalGroup1InRing1 - clearanceIntervalSignalGroup1InRing1;
                allocatedGreenTimeToCurrentStageSignalGroup2InRing1 = allocatedTimeToCurrentStageSignalGroup2InRing1 - clearanceIntervalSignalGroup2InRing1;

                availableOption.signalGroup1 = plannedStages[stageIndex].signalGroup1InRing1;
                availableOption.allocatedTimeStepSignalGroup1 = allocatedTimeToCurrentStageSignalGroup1InRing1;
                availableOption.allocatedGreenTimeSignalGroup1 = allocatedGreenTimeToCurrentStageSignalGroup1InRing1;
                availableOption.minimumGreenTimeSignalGroup1 = gminSignalGroup1InRing1;
                availableOption.clearanceIntervalSignalGroup1 = clearanceIntervalSignalGroup1InRing1;
                availableOption.signalGroup2 = plannedStages[stageIndex].signalGroup2InRing1;
                availableOption.allocatedTimeStepSignalGroup2 = allocatedTimeToCurrentStageSignalGroup2InRing1;
                availableOption.allocatedGreenTimeSignalGroup2 = allocatedGreenTimeToCurrentStageSignalGroup2InRing1;
                availableOption.minimumGreenTimeSignalGroup2 = gminSignalGroup2InRing1;
                availableOption.clearanceIntervalSignalGroup2 = clearanceIntervalSignalGroup2InRing1;

                availableOptionListForForwardStage.push_back(availableOption);
            }
        }
        if (stageIndex == 0)
            activePhasesPerformanceMeasurementValue = getPerformaceMeasurementListForActivePhasesInFirstStage(timeStep);

        else
            activePhasesPerformanceMeasurementValue = getPerformaceMeasurementListForActivePhasesInLaterStage(stageIndex, timeStep, allocatedTimeToPreviousStage, ringNo);
    }

    // Ring2
    if (ringNo == Ring2)
    {
        if (plannedStages[stageIndex].signalGroup1InRing2 == 0)
        {
            if (stageIndex == 0)
                allocatedTimeToCurrentStageSignalGroup2InRing2 = allocatedTimeToCurrentStage - static_cast<int>(trafficControllerStatus[0].initPhase2);

            else
                allocatedTimeToCurrentStageSignalGroup2InRing2 = allocatedTimeToCurrentStage;

            allocatedGreenTimeToCurrentStageSignalGroup2InRing2 = allocatedTimeToCurrentStageSignalGroup2InRing2 - clearanceIntervalSignalGroup2InRing2;

            availableOption.signalGroup1 = 0;
            availableOption.allocatedTimeStepSignalGroup1 = 0;
            availableOption.allocatedGreenTimeSignalGroup1 = 0;
            availableOption.minimumGreenTimeSignalGroup1 = 0;
            availableOption.clearanceIntervalSignalGroup1 = 0;
            availableOption.signalGroup2 = plannedStages[stageIndex].signalGroup2InRing2;
            availableOption.allocatedTimeStepSignalGroup2 = allocatedTimeToCurrentStageSignalGroup2InRing2;
            availableOption.allocatedGreenTimeSignalGroup2 = allocatedGreenTimeToCurrentStageSignalGroup2InRing2;
            availableOption.minimumGreenTimeSignalGroup2 = gminSignalGroup2InRing2;
            availableOption.clearanceIntervalSignalGroup2 = clearanceIntervalSignalGroup2InRing2;

            availableOptionListForForwardStage.push_back(availableOption);
        }

        else if (plannedStages[stageIndex].signalGroup2InRing2 == 0)
        {
            if (stageIndex == 0)
                allocatedTimeToCurrentStageSignalGroup1InRing2 = allocatedTimeToCurrentStage - static_cast<int>(trafficControllerStatus[0].initPhase2);

            else
                allocatedTimeToCurrentStageSignalGroup1InRing2 = allocatedTimeToCurrentStage;

            allocatedGreenTimeToCurrentStageSignalGroup1InRing2 = allocatedTimeToCurrentStageSignalGroup1InRing2 - clearanceIntervalSignalGroup1InRing2;

            availableOption.signalGroup1 = plannedStages[stageIndex].signalGroup1InRing2;
            availableOption.allocatedTimeStepSignalGroup1 = allocatedTimeToCurrentStageSignalGroup1InRing2;
            availableOption.allocatedGreenTimeSignalGroup1 = allocatedGreenTimeToCurrentStageSignalGroup1InRing2;
            availableOption.minimumGreenTimeSignalGroup1 = gminSignalGroup1InRing2;
            availableOption.clearanceIntervalSignalGroup1 = clearanceIntervalSignalGroup1InRing2;
            availableOption.signalGroup2 = 0;
            availableOption.allocatedTimeStepSignalGroup2 = 0;
            availableOption.allocatedGreenTimeSignalGroup2 = 0;
            availableOption.minimumGreenTimeSignalGroup2 = 0;
            availableOption.clearanceIntervalSignalGroup2 = 0;

            availableOptionListForForwardStage.push_back(availableOption);
        }

        else
        {
            signalGroup1AllocatedTimeStepList = getAllocatedTimeStepListToSignalGroup1(allocatedTimeToCurrentStage, currentStageMinimumBarrierLength, gminSignalGroup2InRing2, clearanceIntervalSignalGroup2InRing2);

            for (size_t i = 0; i < signalGroup1AllocatedTimeStepList.size(); i++)
            {
                availableOption.reset();

                if (stageIndex == 0)
                {
                    allocatedTimeToCurrentStageSignalGroup1InRing2 = signalGroup1AllocatedTimeStepList.at(i) - static_cast<int>(trafficControllerStatus[0].initPhase2);
                    allocatedTimeToCurrentStageSignalGroup2InRing2 = allocatedTimeToCurrentStage - allocatedTimeToCurrentStageSignalGroup1InRing2 - static_cast<int>(trafficControllerStatus[0].initPhase2);
                }

                else
                {
                    allocatedTimeToCurrentStageSignalGroup1InRing2 = signalGroup1AllocatedTimeStepList.at(i);
                    allocatedTimeToCurrentStageSignalGroup2InRing2 = allocatedTimeToCurrentStage - allocatedTimeToCurrentStageSignalGroup1InRing2;
                }

                allocatedGreenTimeToCurrentStageSignalGroup1InRing2 = allocatedTimeToCurrentStageSignalGroup1InRing2 - clearanceIntervalSignalGroup1InRing2;
                allocatedGreenTimeToCurrentStageSignalGroup2InRing2 = allocatedTimeToCurrentStageSignalGroup2InRing2 - clearanceIntervalSignalGroup2InRing2;

                availableOption.signalGroup1 = plannedStages[stageIndex].signalGroup1InRing2;
                availableOption.allocatedTimeStepSignalGroup1 = allocatedTimeToCurrentStageSignalGroup1InRing2;
                availableOption.allocatedGreenTimeSignalGroup1 = allocatedGreenTimeToCurrentStageSignalGroup1InRing2;
                availableOption.minimumGreenTimeSignalGroup1 = gminSignalGroup1InRing2;
                availableOption.clearanceIntervalSignalGroup1 = clearanceIntervalSignalGroup1InRing2;
                availableOption.signalGroup2 = plannedStages[stageIndex].signalGroup2InRing2;
                availableOption.allocatedTimeStepSignalGroup2 = allocatedTimeToCurrentStageSignalGroup2InRing2;
                availableOption.allocatedGreenTimeSignalGroup2 = allocatedGreenTimeToCurrentStageSignalGroup2InRing2;
                availableOption.minimumGreenTimeSignalGroup2 = gminSignalGroup2InRing2;
                availableOption.clearanceIntervalSignalGroup2 = clearanceIntervalSignalGroup2InRing2;

                availableOptionListForForwardStage.push_back(availableOption);
            }
        }
        if (stageIndex == 0)
        {
            activePhasesPerformanceMeasurementValue = getPerformaceMeasurementListForActivePhasesInFirstStage(timeStep);
            inactivePhasesPerformanceMeasurementValue = getPerformaceMeasurementListForInactivePhasesInFirstStage(timeStep, inactiveSignalgroupInCurrentStage);
        }

        else
        {
            activePhasesPerformanceMeasurementValue = getPerformaceMeasurementListForActivePhasesInLaterStage(stageIndex, timeStep, allocatedTimeToPreviousStage, ringNo);
            inactivePhasesPerformanceMeasurementValue = getPerformaceMeasurementListForInactivePhasesInLaterStage(stageIndex, timeStep, allocatedTimeToPreviousStage, inactiveSignalgroupInCurrentStage);
        }
    }
}

/*
    - Following method calculates delay for the active phases in first stage
    - For second atcive signal group allocated green time will be sum of allocated timestep to first active signal group and allocated green to second active signal group
*/
double SignalControl::getPerformaceMeasurementListForActivePhasesInFirstStage(int currentStageTimeStep)
{
    double performanceMeasurementForSignalGroup1{};
    double performanceMeasurementForSignalGroup2{};
    double minPerformanceMeasurement{};
    int temporarySignalGroup1{};
    int temporarySignalGroup2{};
    int noOfVehicleInSignalGroup1{};
    int noOfVehicleInSignalGroup2{};

    vector<double> performanceMeasurementList{};

    for (size_t i = 0; i < availableOptionListForForwardStage.size(); i++)
    {
        performanceMeasurementForSignalGroup1 = 0;
        performanceMeasurementForSignalGroup2 = 0;
        temporarySignalGroup1 = availableOptionListForForwardStage[i].signalGroup1;
        temporarySignalGroup2 = availableOptionListForForwardStage[i].signalGroup2;

        for (size_t j = 0; j < vehicleArrivalTable.size(); j++)
        {
            // Delay occurs when vehicle's ETA is less than current stage time step and greater than the allocated green time its desired service phase
            if (vehicleArrivalTable[j].signalGroup == temporarySignalGroup1 && vehicleArrivalTable[j].ETA <= currentStageTimeStep &&
                vehicleArrivalTable[j].ETA > availableOptionListForForwardStage[i].allocatedGreenTimeSignalGroup1)
            {
                performanceMeasurementForSignalGroup1 = performanceMeasurementForSignalGroup1 + (currentStageTimeStep - vehicleArrivalTable[j].ETA) * vehicleArrivalTable[j].priorityWeight;
                noOfVehicleInSignalGroup1++;
            }

            else if (vehicleArrivalTable[j].signalGroup == temporarySignalGroup2 && vehicleArrivalTable[j].ETA <= currentStageTimeStep &&
                     vehicleArrivalTable[j].ETA > availableOptionListForForwardStage[i].allocatedGreenTimeSignalGroup2)
            {
                performanceMeasurementForSignalGroup2 = performanceMeasurementForSignalGroup2 + (currentStageTimeStep - vehicleArrivalTable[j].ETA) * vehicleArrivalTable[j].priorityWeight;
                noOfVehicleInSignalGroup2++;
            }
        }

        if (noOfVehicleInSignalGroup1 > 0 && objectiveFunction == "AverageDelayPerMovement")
            performanceMeasurementForSignalGroup1 = performanceMeasurementForSignalGroup1 / noOfVehicleInSignalGroup1;

        if (noOfVehicleInSignalGroup2 > 0 && objectiveFunction == "AverageDelayPerMovement")
            performanceMeasurementForSignalGroup2 = performanceMeasurementForSignalGroup2 / noOfVehicleInSignalGroup2;

        performanceMeasurementList.push_back(performanceMeasurementForSignalGroup1 + performanceMeasurementForSignalGroup2);
    }

    minPerformanceMeasurement = *min_element(performanceMeasurementList.begin(), performanceMeasurementList.end());
    auto it = find(performanceMeasurementList.begin(), performanceMeasurementList.end(), minPerformanceMeasurement);
    if (it != performanceMeasurementList.end())
        indexOfMinPerformanceMeasurement = static_cast<int>(it - performanceMeasurementList.begin());

    return minPerformanceMeasurement;
}

/*
    - Method to calculate delay for the inactive phases in first stage
*/
double SignalControl::getPerformaceMeasurementListForInactivePhasesInFirstStage(int currentStageTimeStep, vector<int> inactiveSignalGroupIncurrentStage)
{
    double performanceMeasurement{};
    int temporarySignalGroup{};
    int noOfVehicle{};

    for (size_t i = 0; i < inactiveSignalGroupIncurrentStage.size(); i++)
    {
        temporarySignalGroup = inactiveSignalGroupIncurrentStage.at(i);

        for (size_t j = 0; j < vehicleArrivalTable.size(); j++)
        {
            if (vehicleArrivalTable[j].signalGroup == temporarySignalGroup && vehicleArrivalTable[j].ETA <= currentStageTimeStep)
            {
                performanceMeasurement = performanceMeasurement + (currentStageTimeStep - vehicleArrivalTable[j].ETA) * vehicleArrivalTable[j].priorityWeight;
                noOfVehicle++;
            }
        }
    }

    if (noOfVehicle > 0 && objectiveFunction == "AverageDelayPerMovement")
        performanceMeasurement = performanceMeasurement / noOfVehicle;

    return performanceMeasurement;
}
/*
    - Following method calculates delay for the actives phases if it is not first stage
    - Before calculating delay, the method calculates the served green time in the previous stages for active phases of current stage
    - If performanceMeasurementTable's stageNo matches with current stage's index and time step macthes with allocatedTimeToPreviousStage,
        - performanceMeasurementTable's signal group doesn't macth with the current stage's sigal group break the loop.
        - performanceMeasurementTable's signal group macthes with the current stage's signal group it is required to consider allocated time and allocated green time while estimating delay
*/
double SignalControl::getPerformaceMeasurementListForActivePhasesInLaterStage(int stageIndex, int currentStageTimeStep, int allocatedTimeToPreviousStage, int ringNo)
{
    double performanceMeasurementForSignalGroup1{};
    double performanceMeasurementForSignalGroup2{};
    double minPerformanceMeasurement{};
    int noOfVehicleInSignalGroup1{};
    int noOfVehicleInSignalGroup2{};
    int temporarySignalGroup1{};
    int temporarySignalGroup2{};
    int allocatedTimeToSignalGroup1{};
    int allocatedTimeToSignalGroup2{};
    int allocatedGreenTimeToSignalGroup1InPreviousStage{};
    int allocatedGreenTimeToSignalGroup2InPreviousStage{};
    int allocatedTimeToSignalGroup1InPreviousStage = allocatedTimeToPreviousStage;
    int allocatedTimeToSignalGroup2InPreviousStage = allocatedTimeToPreviousStage;

    vector<double> performanceMeasurementList{};

    if (ringNo == Ring1)
    {
        temporarySignalGroup1 = plannedStages[stageIndex].signalGroup1InRing1;
        temporarySignalGroup2 = plannedStages[stageIndex].signalGroup2InRing1;

        // Compute the allocated phase duration and allocated green time of signalGroup1InRing1 in previous stages
        for (int index = stageIndex - 1; index >= 0; --index)
        {
            for (size_t j = 0; j < performanceMeasurementTable.size(); j++)
            {
                /*
                    - If starting phase is phase 2 & 6, for leading left turn stages will be {{2,6},{3,4,7,8},{1,2,5,6},{3,4,7,8}}
                    - At stage 3, while calculating served green time in the previous stages for phase 2, it requires to match signalGroup1InRing1 at stage1 not signalGroup2InRing1
                */
                if (performanceMeasurementTable[j].stageNo == (index + 1) &&
                    ((performanceMeasurementTable[j].signalGroup1InRing1 == temporarySignalGroup1) || (performanceMeasurementTable[j].signalGroup2InRing1 == temporarySignalGroup1)) &&
                    performanceMeasurementTable[j].timeStep == allocatedTimeToSignalGroup1InPreviousStage)
                {
                    if (performanceMeasurementTable[j].signalGroup1InRing1 == temporarySignalGroup1)
                    {
                        allocatedTimeToSignalGroup1 = allocatedTimeToSignalGroup1 + performanceMeasurementTable[j].allocatedTimeStepSignalGroup1Ring1;
                        allocatedGreenTimeToSignalGroup1InPreviousStage = allocatedGreenTimeToSignalGroup1InPreviousStage + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup1Ring1;
                    }

                    else if (performanceMeasurementTable[j].signalGroup2InRing1 == temporarySignalGroup1)
                    {
                        allocatedTimeToSignalGroup1 = allocatedTimeToSignalGroup1 + performanceMeasurementTable[j].allocatedTimeStepSignalGroup2Ring1;
                        allocatedGreenTimeToSignalGroup1InPreviousStage = allocatedGreenTimeToSignalGroup1InPreviousStage + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup2Ring1;
                    }
                    // If solving for 6stage then allocatedTimeToPreviousStage at stage 4 will be required for the calculation of stage 2
                    allocatedTimeToSignalGroup1InPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                    break;
                }

                else if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup1InRing1 != temporarySignalGroup1 &&
                         performanceMeasurementTable[j].signalGroup2InRing1 != temporarySignalGroup1 && performanceMeasurementTable[j].timeStep == allocatedTimeToSignalGroup1InPreviousStage)
                    allocatedTimeToSignalGroup1InPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
            }

            // Compute the allocated phase duration and allocated green time of signalGroup1InRing1 in previous stages
            for (size_t k = 0; k < performanceMeasurementTable.size(); k++)
            {
                if (performanceMeasurementTable[k].stageNo == (index + 1) &&
                    ((performanceMeasurementTable[k].signalGroup1InRing1 == temporarySignalGroup2) || (performanceMeasurementTable[k].signalGroup2InRing1 == temporarySignalGroup2)) &&
                    performanceMeasurementTable[k].timeStep == allocatedTimeToSignalGroup2InPreviousStage)
                {
                    if (performanceMeasurementTable[k].signalGroup1InRing1 == temporarySignalGroup2)
                    {
                        allocatedTimeToSignalGroup2 = allocatedTimeToSignalGroup2 + performanceMeasurementTable[k].allocatedTimeStepSignalGroup1Ring1;
                        allocatedGreenTimeToSignalGroup2InPreviousStage = allocatedGreenTimeToSignalGroup2InPreviousStage + performanceMeasurementTable[k].allocatedGreenTimeSignalGroup1Ring1;
                    }

                    else if (performanceMeasurementTable[k].signalGroup2InRing1 == temporarySignalGroup2)
                    {
                        allocatedTimeToSignalGroup2 = allocatedTimeToSignalGroup2 + performanceMeasurementTable[k].allocatedTimeStepSignalGroup2Ring1;
                        allocatedGreenTimeToSignalGroup2InPreviousStage = allocatedGreenTimeToSignalGroup2InPreviousStage + performanceMeasurementTable[k].allocatedGreenTimeSignalGroup2Ring1;
                    }

                    allocatedTimeToSignalGroup2InPreviousStage = performanceMeasurementTable[k].allocatedTimeToPreviousStage;
                    break;
                }

                else if (performanceMeasurementTable[k].stageNo == (index + 1) && performanceMeasurementTable[k].signalGroup1InRing1 != temporarySignalGroup2 &&
                         performanceMeasurementTable[k].signalGroup2InRing1 != temporarySignalGroup2 &&
                         performanceMeasurementTable[k].timeStep == allocatedTimeToSignalGroup2InPreviousStage)
                    allocatedTimeToSignalGroup2InPreviousStage = performanceMeasurementTable[k].allocatedTimeToPreviousStage;
            }
        }
    }

    else if (ringNo == Ring2)
    {
        temporarySignalGroup1 = plannedStages[stageIndex].signalGroup1InRing2;
        temporarySignalGroup2 = plannedStages[stageIndex].signalGroup2InRing2;

        for (int index = stageIndex - 1; index >= 0; --index)
        {
            for (size_t j = 0; j < performanceMeasurementTable.size(); j++)
            {
                if (performanceMeasurementTable[j].stageNo == (index + 1) &&
                    ((performanceMeasurementTable[j].signalGroup1InRing2 == temporarySignalGroup1) || (performanceMeasurementTable[j].signalGroup2InRing2 == temporarySignalGroup1)) &&
                    performanceMeasurementTable[j].timeStep == allocatedTimeToSignalGroup1InPreviousStage)
                {
                    if (performanceMeasurementTable[j].signalGroup1InRing2 == temporarySignalGroup1)
                    {
                        allocatedTimeToSignalGroup1 = allocatedTimeToSignalGroup1 + performanceMeasurementTable[j].allocatedTimeStepSignalGroup1Ring2;
                        allocatedGreenTimeToSignalGroup1InPreviousStage = allocatedGreenTimeToSignalGroup1InPreviousStage + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup1Ring2;
                    }

                    else if (performanceMeasurementTable[j].signalGroup2InRing2 == temporarySignalGroup1)
                    {
                        allocatedTimeToSignalGroup1 = allocatedTimeToSignalGroup1 + performanceMeasurementTable[j].allocatedTimeStepSignalGroup2Ring2;
                        allocatedGreenTimeToSignalGroup1InPreviousStage = allocatedGreenTimeToSignalGroup1InPreviousStage + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup2Ring2;
                    }

                    allocatedTimeToSignalGroup1InPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                    break;
                }

                else if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup1InRing2 != temporarySignalGroup1 &&
                         performanceMeasurementTable[j].signalGroup2InRing2 != temporarySignalGroup1 &&
                         performanceMeasurementTable[j].timeStep == allocatedTimeToSignalGroup1InPreviousStage)
                    allocatedTimeToSignalGroup1InPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
            }

            for (size_t k = 0; k < performanceMeasurementTable.size(); k++)
            {
                if (performanceMeasurementTable[k].stageNo == (index + 1) &&
                    ((performanceMeasurementTable[k].signalGroup1InRing2 == temporarySignalGroup2) || (performanceMeasurementTable[k].signalGroup2InRing2 == temporarySignalGroup2)) &&
                    performanceMeasurementTable[k].timeStep == allocatedTimeToSignalGroup2InPreviousStage)
                {
                    if (performanceMeasurementTable[k].signalGroup1InRing2 == temporarySignalGroup2)
                    {
                        allocatedTimeToSignalGroup2 = allocatedTimeToSignalGroup2 + performanceMeasurementTable[k].allocatedTimeStepSignalGroup1Ring2;
                        allocatedGreenTimeToSignalGroup2InPreviousStage = allocatedGreenTimeToSignalGroup2InPreviousStage + performanceMeasurementTable[k].allocatedGreenTimeSignalGroup1Ring2;
                    }

                    else if (performanceMeasurementTable[k].signalGroup2InRing2 == temporarySignalGroup2)
                    {
                        allocatedTimeToSignalGroup2 = allocatedTimeToSignalGroup2 + performanceMeasurementTable[k].allocatedTimeStepSignalGroup2Ring2;
                        allocatedGreenTimeToSignalGroup2InPreviousStage = allocatedGreenTimeToSignalGroup2InPreviousStage + performanceMeasurementTable[k].allocatedGreenTimeSignalGroup2Ring2;
                    }

                    allocatedTimeToSignalGroup2InPreviousStage = performanceMeasurementTable[k].allocatedTimeToPreviousStage;
                    break;
                }

                else if (performanceMeasurementTable[k].stageNo == (index + 1) && performanceMeasurementTable[k].signalGroup1InRing2 != temporarySignalGroup2 &&
                         performanceMeasurementTable[k].signalGroup2InRing2 != temporarySignalGroup2 &&
                         performanceMeasurementTable[k].timeStep == allocatedTimeToSignalGroup2InPreviousStage)
                    allocatedTimeToSignalGroup2InPreviousStage = performanceMeasurementTable[k].allocatedTimeToPreviousStage;
            }
        }
    }

    // Calculate Performance Measurement for active phases in current stage
    for (size_t i = 0; i < availableOptionListForForwardStage.size(); i++)
    {
        performanceMeasurementForSignalGroup1 = 0.0;
        performanceMeasurementForSignalGroup2 = 0.0;
        temporarySignalGroup1 = availableOptionListForForwardStage[i].signalGroup1;
        temporarySignalGroup2 = availableOptionListForForwardStage[i].signalGroup2;

        for (size_t j = 0; j < vehicleArrivalTable.size(); j++)
        {
            if (vehicleArrivalTable[j].signalGroup == temporarySignalGroup1 && vehicleArrivalTable[j].ETA <= currentStageTimeStep &&
                vehicleArrivalTable[j].ETA > (availableOptionListForForwardStage[i].allocatedGreenTimeSignalGroup1 + allocatedGreenTimeToSignalGroup1InPreviousStage))
            {
                performanceMeasurementForSignalGroup1 = performanceMeasurementForSignalGroup1 + (currentStageTimeStep - vehicleArrivalTable[j].ETA) * vehicleArrivalTable[j].priorityWeight;
                noOfVehicleInSignalGroup1++;
            }

            else if (vehicleArrivalTable[j].signalGroup == temporarySignalGroup2 && vehicleArrivalTable[j].ETA <= currentStageTimeStep &&
                     vehicleArrivalTable[j].ETA > (availableOptionListForForwardStage[i].allocatedGreenTimeSignalGroup2 + allocatedGreenTimeToSignalGroup2InPreviousStage))
            {
                performanceMeasurementForSignalGroup2 = performanceMeasurementForSignalGroup2 + (currentStageTimeStep - vehicleArrivalTable[j].ETA) * vehicleArrivalTable[j].priorityWeight;
                noOfVehicleInSignalGroup2++;
            }
        }

        if (noOfVehicleInSignalGroup1 > 0 && objectiveFunction == "AverageDelayPerMovement")
            performanceMeasurementForSignalGroup1 = performanceMeasurementForSignalGroup1 / noOfVehicleInSignalGroup1;

        if (noOfVehicleInSignalGroup2 > 0 && objectiveFunction == "AverageDelayPerMovement")
            performanceMeasurementForSignalGroup2 = performanceMeasurementForSignalGroup2 / noOfVehicleInSignalGroup2;

        performanceMeasurementList.push_back(performanceMeasurementForSignalGroup1 + performanceMeasurementForSignalGroup2);
    }

    minPerformanceMeasurement = *min_element(performanceMeasurementList.begin(), performanceMeasurementList.end());
    
    auto it = find(performanceMeasurementList.begin(), performanceMeasurementList.end(), minPerformanceMeasurement);
    if (it != performanceMeasurementList.end())
        indexOfMinPerformanceMeasurement = static_cast<int>(it - performanceMeasurementList.begin());

    return minPerformanceMeasurement;
}

/*
    - Method to calculates delay for the inactive phases in later stage
*/
double SignalControl::getPerformaceMeasurementListForInactivePhasesInLaterStage(int stageIndex, int currentStageTimeStep, int allocatedTimeToPreviousStage, vector<int> inactiveSignalGroupIncurrentStage)
{
    double performanceMeasurement{};
    int temporarySignalGroup{};
    int temporaryAllocatedTimeToPreviousStage{};
    int allocatedTime{};
    int allocatedGreenTime{};
    int noOfVehicle{};

    for (size_t i = 0; i < inactiveSignalGroupIncurrentStage.size(); i++)
    {
        temporarySignalGroup = inactiveSignalGroupIncurrentStage.at(i);
        temporaryAllocatedTimeToPreviousStage = allocatedTimeToPreviousStage;
        allocatedTime = 0;
        allocatedGreenTime = 0;

        // Compute the total amount of time (phase duration and green time) allocated to a phase which is inactive in current stage
        for (int index = stageIndex - 1; index >= 0; --index)
        {
            for (size_t j = 0; j < performanceMeasurementTable.size(); j++)
            {
                if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup1InRing1 != temporarySignalGroup &&
                    performanceMeasurementTable[j].signalGroup2InRing1 != temporarySignalGroup && performanceMeasurementTable[j].signalGroup1InRing2 != temporarySignalGroup &&
                    performanceMeasurementTable[j].signalGroup2InRing2 != temporarySignalGroup && performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage)
                {
                    temporaryAllocatedTimeToPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                    break;
                }

                else if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup1InRing1 == temporarySignalGroup &&
                         performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage && performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage)
                {
                    allocatedTime = allocatedTime + performanceMeasurementTable[j].allocatedTimeStepSignalGroup1Ring1;
                    allocatedGreenTime = allocatedGreenTime + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup1Ring1;
                    temporaryAllocatedTimeToPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                }

                else if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup2InRing1 == temporarySignalGroup &&
                         performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage && performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage)
                {
                    allocatedTime = allocatedTime + performanceMeasurementTable[j].allocatedTimeStepSignalGroup2Ring1;
                    allocatedGreenTime = allocatedGreenTime + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup2Ring1;
                    temporaryAllocatedTimeToPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                }

                else if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup1InRing2 == temporarySignalGroup &&
                         performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage && performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage)
                {
                    allocatedTime = allocatedTime + performanceMeasurementTable[j].allocatedTimeStepSignalGroup1Ring2;
                    allocatedGreenTime = allocatedGreenTime + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup1Ring2;
                    temporaryAllocatedTimeToPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                }

                else if (performanceMeasurementTable[j].stageNo == (index + 1) && performanceMeasurementTable[j].signalGroup2InRing2 == temporarySignalGroup &&
                         performanceMeasurementTable[j].timeStep == temporaryAllocatedTimeToPreviousStage)
                {
                    allocatedTime = allocatedTime + performanceMeasurementTable[j].allocatedTimeStepSignalGroup2Ring2;
                    allocatedGreenTime = allocatedGreenTime + performanceMeasurementTable[j].allocatedGreenTimeSignalGroup2Ring2;
                    temporaryAllocatedTimeToPreviousStage = performanceMeasurementTable[j].allocatedTimeToPreviousStage;
                }
            }
        }

        for (size_t j = 0; j < vehicleArrivalTable.size(); j++)
        {
            if (vehicleArrivalTable[j].signalGroup == temporarySignalGroup && vehicleArrivalTable[j].ETA > allocatedGreenTime &&
                vehicleArrivalTable[j].ETA <= currentStageTimeStep)
            {
                performanceMeasurement = performanceMeasurement + (currentStageTimeStep - vehicleArrivalTable[j].ETA) * vehicleArrivalTable[j].priorityWeight;
                noOfVehicle++;
            }
        }
    }

    if (noOfVehicle > 0 && objectiveFunction == "AverageDelayPerMovement")
        performanceMeasurement = performanceMeasurement / noOfVehicle;

    return performanceMeasurement;
}

/*
    - Following method calculates all the possible options to serve signalgroup1 on current stage.
    - The method also ensures a phase will be served at least for its minimum green time.
*/
vector<int> SignalControl::getAllocatedTimeStepListToSignalGroup1(int allocatedTotalTimeStep, int currentStageMinimumBarrierLength, int minimumGreenTimeSignalGroup2, int clearanceIntervalSignalGroup2)
{
    vector<int> signalGroup1AllocatedTimeStepList{};

    for (int i = 0; i <= allocatedTotalTimeStep; i++)
    {
        if (i >= currentStageMinimumBarrierLength)
            signalGroup1AllocatedTimeStepList.push_back(i - (minimumGreenTimeSignalGroup2 + clearanceIntervalSignalGroup2));
    }

    return signalGroup1AllocatedTimeStepList;
}

/*
    - Follwing method calculates all the possible options (time) to allocate time in previous stage
    - The method ensures previous stages are served minimum green time
*/
vector<int> SignalControl::getPreviousStageTimeStep(int currentStageTimeStep, int currentStageMinimumBarrierLength)
{
    vector<int> previousStageTimeStepList{};
    int sumOfMinimumTimeRequirementForPreviousStage{};

    for (size_t i = 0; i < previousStageMinimumBarrierLengthList.size(); i++)
        sumOfMinimumTimeRequirementForPreviousStage = sumOfMinimumTimeRequirementForPreviousStage + previousStageMinimumBarrierLengthList[i];

    for (int timeStep = 0; timeStep <= currentStageTimeStep; timeStep++)
    {
        if (timeStep >= (sumOfMinimumTimeRequirementForPreviousStage + currentStageMinimumBarrierLength))
            previousStageTimeStepList.insert(previousStageTimeStepList.begin(), (timeStep - currentStageMinimumBarrierLength));
    }

    if (previousStageTimeStepList.empty())
        previousStageTimeStepList.push_back(sumOfMinimumTimeRequirementForPreviousStage);

    return previousStageTimeStepList;
}

/*
    - Method to calculate value function for previous stage
*/
double SignalControl::getCummulativeValueFunction(int currentStageNo, int allocatedTimeToPreviousStage)
{
    double previousStageValueFunction{};

    for (size_t i = 0; i < performanceMeasurementTable.size(); i++)
    {
        if (performanceMeasurementTable[i].stageNo == (currentStageNo - 1) && performanceMeasurementTable[i].timeStep == allocatedTimeToPreviousStage)
        {
            previousStageValueFunction = performanceMeasurementTable[i].totalPerformmanceMeasurement;
            break;
        }
    }

    return previousStageValueFunction;
}

/*
    - Method to check stopping rule for forward recurssion;
*/
bool SignalControl::checkStoppingCrietria(double previousPerformanceMeasurement, double currentPerformanceMeasurement)
{
    bool stopIteration{};

    if (previousPerformanceMeasurement < currentPerformanceMeasurement)
        stopIteration = true;

    return stopIteration;
}

/*
    - Method to print optimal plan
*/
void SignalControl::printOptimalPan()
{
    cout << "StageNo"
         << " "
         << "TimeStep"
         << " "
         << "SG1R1"
         << " "
         << "SG2R1"
         << " "
         << "SG1R2"
         << " "
         << "SG2R2"
         << " "
         << "GT_SG1Ring1"
         << " "
         << "GT_SG2Ring1"
         << " "
         << "GT_SG1Ring2"
         << " "
         << "GT_SG2Ring2"
         << " "
         << "PerformanceMeasurement" << endl;

    for (size_t i = 0; i < optimalSignalTiming.size(); i++)
        cout << optimalSignalTiming[i].stageNo << "          " << optimalSignalTiming[i].timeStep << "      "
             << optimalSignalTiming[i].signalGroup1InRing1 << "     " << optimalSignalTiming[i].signalGroup2InRing1 << "     "
             << optimalSignalTiming[i].signalGroup1InRing2 << "     " << optimalSignalTiming[i].signalGroup2InRing2 << "      "
             << optimalSignalTiming[i].allocatedGreenTimeSignalGroup1Ring1 << "            " << optimalSignalTiming[i].allocatedGreenTimeSignalGroup2Ring1 << "            "
             << optimalSignalTiming[i].allocatedGreenTimeSignalGroup1Ring2 << "            " << optimalSignalTiming[i].allocatedGreenTimeSignalGroup2Ring2 << "            "
             << optimalSignalTiming[i].optimalPerformmanceMeasurement << endl;
}

/*
    - Method to store forward recurssion results in a csv file.
*/
void SignalControl::logData()
{
    string logFileName{};
    ofstream logFile;
    time_t now = time(0);
    struct tm tstruct;
    char logFileOpenningTime[80];
    tstruct = *localtime(&now);
    strftime(logFileOpenningTime, sizeof(logFileOpenningTime), "%m%d%Y_%H%M%S", &tstruct);

    logFileName = "/nojournal/bin/log/PerformanceMeasurementTable.csv";
    logFile.open(logFileName);
    
    logFile << "Stage No"
            << ","
            << "Time Step"
            << ","
            << "SG1 Ring1"
            << ","
            << "SG2 Ring1"
            << ","
            << "SG1 Ring2"
            << ","
            << "SG2 Ring2"
            << ","
            << "AT SG1 Ring1"
            << ","
            << "AT SG2 Ring1"
            << ","
            << "AT SG1 Ring2"
            << ","
            << "AT SG2 Ring2"
            << ","
            << "GT SG1 Ring1"
            << ","
            << "GT SG2 Ring1"
            << ","
            << "GT SG1 Ring2"
            << ","
            << "GT SG2 Ring2"
            << ","
            << "AT Previous Stage"
            << ","
            << "Active Phases PM"
            << ","
            << "Inactive Phases PM"
            << ","
            << "Previous Stage PM"
            << ","
            << "Total PM"
            << endl;

    for (size_t i = 0; i < performanceMeasurementTable.size(); i++)
    {
        logFile << performanceMeasurementTable[i].stageNo << "," << performanceMeasurementTable[i].timeStep << ","
                << performanceMeasurementTable[i].signalGroup1InRing1 << "," << performanceMeasurementTable[i].signalGroup2InRing1 << ","
                << performanceMeasurementTable[i].signalGroup1InRing2 << "," << performanceMeasurementTable[i].signalGroup2InRing2 << ","
                << performanceMeasurementTable[i].allocatedTimeStepSignalGroup1Ring1 << "," << performanceMeasurementTable[i].allocatedTimeStepSignalGroup2Ring1 << ","
                << performanceMeasurementTable[i].allocatedTimeStepSignalGroup1Ring2 << "," << performanceMeasurementTable[i].allocatedTimeStepSignalGroup2Ring2 << ","
                << performanceMeasurementTable[i].allocatedGreenTimeSignalGroup1Ring1 << "," << performanceMeasurementTable[i].allocatedGreenTimeSignalGroup2Ring1 << ","
                << performanceMeasurementTable[i].allocatedGreenTimeSignalGroup1Ring2 << "," << performanceMeasurementTable[i].allocatedGreenTimeSignalGroup2Ring2 << ","
                << performanceMeasurementTable[i].allocatedTimeToPreviousStage << "," << performanceMeasurementTable[i].activePhasesPerformancemeasurement << ","
                << performanceMeasurementTable[i].inactivePhasesPerformancemeasurement << "," << performanceMeasurementTable[i].previousStagePerformanceMeasurement << ","
                << performanceMeasurementTable[i].totalPerformmanceMeasurement << "," << endl;
    }
}

vector<COP::OptimalPlan> SignalControl::getOptimalSignalTiming()
{
    return optimalSignalTiming;
}

SignalControl::~SignalControl()
{
}