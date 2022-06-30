/***********************************************************************************
 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
**********************************************************************************
  SignalControl.h
  Created by: Debashis Das
  University of Arizona
  College of Engineering
  This code was developed under the supervision of Professor Larry Head
  in the Systems and Industrial Engineering Department.
  Revision History:
  1. Header file for SignalControl class
*/
#pragma once
#include <iostream>
#include <iomanip>
#include <vector>
#include <fstream>

#include "ArrivalTable.h"
#include "COP.h"
#include "ScheduleManager.h"
#include "StageManager.h"
#include "TrafficSignalPlan.h"
#include "Timestamp.h"
#include "json/json.h"
#include "msgEnum.h"

using namespace MsgEnum;

#define Ring1 1
#define Ring2 2
#define Phase1 1
#define Phase2 2
#define Phase3 3
#define Phase4 4
#define Green_Time 1
#define Clearance_Interval 2
#define FirstPhaseOfRing1 1
#define FirstPhaseOfRing2 5
#define LastPhaseOfRing1 4
#define LastPhaseOfRing2 8
#define NumberOfPhasePerRing 4
#define NumberOfStartingPhase 2
#define Initialize 0.0
#define BigNumber 10000

using std::cout;
using std::endl;
using std::fixed;
using std::ifstream;
using std::ofstream;
using std::setprecision;
using std::showpoint;
using std::string;
using std::vector;

class SignalControl
{
private:
  int timeHorizon{120};
  int indexOfMinPerformanceMeasurement{};
  int gminSignalGroup1InRing1{};
  int gminSignalGroup2InRing1{};
  int gminSignalGroup1InRing2{};
  int gminSignalGroup2InRing2{};
  int clearanceIntervalSignalGroup1InRing1{};
  int clearanceIntervalSignalGroup2InRing1{};
  int clearanceIntervalSignalGroup1InRing2{};
  int clearanceIntervalSignalGroup2InRing2{};
  int allocatedTimeToCurrentStageSignalGroup1InRing1{};
  int allocatedTimeToCurrentStageSignalGroup2InRing1{};
  int allocatedTimeToCurrentStageSignalGroup1InRing2{};
  int allocatedTimeToCurrentStageSignalGroup2InRing2{};
  int allocatedGreenTimeToCurrentStageSignalGroup1InRing1{};
  int allocatedGreenTimeToCurrentStageSignalGroup2InRing1{};
  int allocatedGreenTimeToCurrentStageSignalGroup1InRing2{};
  int allocatedGreenTimeToCurrentStageSignalGroup2InRing2{};
  double activePhasesPerformanceMeasurementValue{};
  double inactivePhasesPerformanceMeasurementValue{};
  string scheduleJsonString{};
  string objectiveFunction{};
  vector<int> previousStageSignalGroupList{};
  vector<int> previousStageMinimumTimeList{};
  vector<int> plannedSignalGroupInRing1{};
  vector<int> plannedSignalGroupInRing2{};
  vector<int> activeSignalGroup{};
  vector<int> inactiveSignalGroup{};
  vector<int> P11{};
  vector<int> P12{};
  vector<int> P13{};
  vector<int> P14{};
  vector<int> P21{};
  vector<int> P22{};
  vector<int> P23{};
  vector<int> P24{};
  vector<int> previousStageMinimumBarrierLengthList{};
  vector<int> vehicleCallList{};
  vector<int> pedCallList{};
  vector<int> phaseCallList{};
  vector<int> arrivalPhaseCallList{};
  vector<TrafficControllerData::TrafficSignalPlan> trafficSignalPlan{};
  vector<TrafficControllerData::TrafficConrtollerStatus> trafficControllerStatus{};
  vector<ArrivalTable> vehicleArrivalTable{};
  vector<COP::plannedSignaBarrierGroup> plannedSignalBarrierGroupList{};
  vector<COP::StagePlan> plannedStages{};
  vector<COP::AvailableOptionForForwardStage> availableOptionListForForwardStage{};
  vector<COP::PerformanceMeasurement> performanceMeasurementTable{};
  vector<COP::OptimalPlan> optimalSignalTiming{};

public:
  SignalControl(string objective_Function, int time_Horizon,
                vector<TrafficControllerData::TrafficSignalPlan> signalPlan, vector<TrafficControllerData::TrafficConrtollerStatus> signalStatus, vector<int> phase_Call_List,
                vector<ArrivalTable> vehicle_Arrival_Table, vector<int> arrival_PhaseCall_List,
                vector<int> P_11, vector<int> P_12, vector<int> P_13, vector<int> P_14,
                vector<int> P_21, vector<int> P_22, vector<int> P_23, vector<int> P_24);
  ~SignalControl();

  void getActiveSignalGroup();
  void currentStageRecurssion(int stageIndex, int timeStep, int allocatedTimeToCurrentStage, int currentStageMinimumBarrierLength, int allocatedTimeToPreviousStage, vector<int> inactiveSignalgroupInCurrentStage, int ringNo);
  void signalControlAlgorithmForwardRecursion();
  void signalControlAlgorithmBackwardRecursion();
  void printOptimalPan();
  void logData();
  int getMinimumBarrierLength(int stageIndex);
  double getPerformaceMeasurementListForActivePhasesInFirstStage(int currentStageTimeStep);
  double getPerformaceMeasurementListForInactivePhasesInFirstStage(int currentStageTimeStep, vector<int> inactiveSignalGroupIncurrentStage);
  double getPerformaceMeasurementListForActivePhasesInLaterStage(int stageIndex, int currentStageTimeStep, int allocatedTimeToPreviousStage, int ringNo);
  double getPerformaceMeasurementListForInactivePhasesInLaterStage(int stageIndex, int currentStageTimeStep, int allocatedTimeToPreviousStage, vector<int> inactiveSignalGroupIncurrentStage);
  double getCummulativeValueFunction(int currentStageNo, int allocatedTimeToPreviousStage);
  bool checkStoppingCrietria(double previousPerformanceMeasurement, double currentPerformanceMeasurement);
  vector<int> getPreviousStageTimeStep(int currentStageTimeStep, int currentStageMinimumBarrierLength);
  vector<int> getAllocatedTimeStepListToSignalGroup1(int allocatedTotalTimeStep, int currentStageMinimumBarrierLength, int minimumGreenTimeSignalGroup2, int clearanceIntervalSignalGroup2);
  vector<COP::OptimalPlan> getOptimalSignalTiming();
};
