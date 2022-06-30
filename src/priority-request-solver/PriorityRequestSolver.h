/***********************************************************************************
 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
**********************************************************************************
  PriorityRequestSolver.h
  Created by: Debashis Das
  University of Arizona   
  College of Engineering
  This code was developed under the supervision of Professor Larry Head
  in the Systems and Industrial Engineering Department.
  Revision History:
  1. Header file for PriorityRequestSolver class
*/

#pragma once

#include <sstream>
#include "ScheduleManager.h"
#include "SolverDataManager.h"
#include "OptimizationModelManager.h"
#include "TrafficConrtollerStatusManager.h"
#include "SignalControl.h"

using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::ofstream;
using std::string;
using std::stringstream;
using std::vector;

#define SignalCoordinationVehicleType 20
#define MinuteToSecondCoversion 60.0
#define HourToSecondConversion 3600.0
// #define emergencyVehicle 2
// #define transit 6
// #define truck 9
#define CoordinationVehicleType 20

enum msgType
{
  priorityRequest = 1,
  clearRequest = 2,
  signalPlan = 3,
  currentPhaseStatus = 4,
  splitData = 5,
  arrivalTableData = 6
};

class PriorityRequestSolver
{
private:
  int noOfPhase{};
  int noOfEVInList{};
  int coordinatedPhase1{};
  int coordinatedPhase2{};
  int emergencyVehicle{};
  int transit{};
  int truck{};
  int connectedPassengerVehicle{};
  int nonConnectedVehicle{};
  int timeHorizonForDP{120};
  bool emergencyVehicleStatus{};
  bool signalCoordinationRequestStatus{};
  bool transitOrTruckRequestStatus{};
  bool optimalSolutionStatus{};
  bool logging{};
  bool consoleOutput{};
  double emergencyVehicleWeight{1.0};
  double emergencyVehicleSplitPhaseWeight{0.1};
  double transitWeight{1.0};
  double truckWeight{1.0};
  double dilemmaZoneRequestWeight{20.0};
  double coordinationWeight{0.1};
  double flexibilityWeight{0.01};
  double connectedPassengerVehicleWeight{0.1};
  double nonConnectedVehicleWeight{0.1};
  double MaximumGreen{};
  double cycleLength{};
  double offset{};
  double coordinationStartTime{};
  double priorityWeightsCheckedTime{};
  double earlyReturnedValue1{0.0};
  double earlyReturnedValue2{0.0};
  string scheduleJsonString{};
  // string timePhaseDiagramJsonStringForDP{};
  string logFileName{};
  string optimizationMethod{};
  string objectiveFunction{};
  ofstream logFile;

  vector<RequestList> priorityRequestList{};
  vector<RequestList> dilemmaZoneRequestList{};
  vector<TrafficControllerData::TrafficConrtollerStatus> trafficControllerStatus{};
  vector<TrafficControllerData::TrafficSignalPlan> trafficSignalPlan{};
  vector<TrafficControllerData::TrafficSignalPlan> trafficSignalPlan_EV{};
  vector<TrafficControllerData::TrafficSignalPlan> trafficSignalPlan_SignalCoordination{};
  vector<COP::OptimalPlan> optimalSignalTiming{};
  vector<ArrivalTable> vehicleArrivalTable{};
  vector<int> arrivalPhaseCallList{};
  vector<int> PhaseNumber{};
  vector<double> PedWalk{};
  vector<double> PedClear{};
  vector<double> MinGreen{};
  vector<double> Passage{};
  vector<double> MaxGreen{};
  vector<double> YellowChange{};
  vector<double> RedClear{};
  vector<int> PhaseRing{};
  vector<int> P11{};
  vector<int> P12{};
  vector<int> P13{};
  vector<int> P14{};
  vector<int> P21{};
  vector<int> P22{};
  vector<int> P23{};
  vector<int> P24{};
  vector<int> EV_P11{};
  vector<int> EV_P12{};
  vector<int> EV_P21{};
  vector<int> EV_P22{};
  vector<int> requestedSignalGroup{};
  vector<int> plannedEVPhases{};
  vector<int> conflictingPedCallList{};
  vector<int> dummyPhasesList{};
  vector<int> phaseCallList{};

public:
  PriorityRequestSolver();
  ~PriorityRequestSolver();

  void createPriorityRequestList(string jsonString);
  void setDilemmaZoneRequesStatus();
  void modifyPriorityRequestList();
  void modifySignalTimingPlan();
  void modifyCoordinationSignalTimingPlan();
  void managePriorityRequestListForEV();
  void setSignalBarrierGroup();
  void GLPKSolver();
  void setCurrentSignalTimingPlan(string jsonString);
  void setSignalCoordinationTimingPlan(string jsonString);
  void setOptimizationInput();
  void getRequestedSignalGroup();
  void getEVPhases();
  void getEVTrafficSignalPlan();
  void getCurrentSignalStatus(string jsonString);
  void validateEVTrafficSignalPlan();
  void getPriorityWeightsAndMethod();
  void getDummyPhases();
  void readConfigFile();
  void loggingData(string logString);
  void displayConsoleData(string consoleString);
  void loggingOptimizationData();
  void printSignalPlan();
  void setArrivalTable(string jsonString);
  string getScheduleforTCI();
  string getClearCommandScheduleforTCI();
  string getSignalTimingPlanRequestString();
  string getCurrentSignalStatusRequestString();
  string getSignalCoordinationTimingPlanRequestString();
  string getTimePhaseDiagramMessageString();
  // string getTimePhaseDiagramMessageStringForDP();
  string getOptimizationMethod();
  int getMessageType(string jsonString);
  double getCoefficientOfFrictionValue(double vehicleSpeed);
  double getCurrentTime();
  bool findEmergencyVehicleRequestInList();
  bool findTransitOrTruckRequestInList();
  bool findCoordinationRequestInList();
  bool getOptimalSolutionValidationStatus();
  bool checkTrafficSignalTimingPlanStatus();
  bool checkSignalCoordinationTimingPlanStatus();
  bool checkUpdatesForPriorityWeights();
};