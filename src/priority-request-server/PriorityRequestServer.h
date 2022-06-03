/*
**********************************************************************************
 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.
**********************************************************************************
  PriorrityRequestServer.h
  Created by: Debashis Das
  University of Arizona   
  College of Engineering
  This code was developed under the supervision of Professor Larry Head
  in the Systems and Industrial Engineering Department.
  Revision History:
  1. This script is the header file for PriorrityRequestServer.cpp
*/

#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include "ArrivalTable.h"
#include "SignalRequest.h"
#include "SignalStatus.h"
#include "ActiveRequest.h"
#include "Timestamp.h"
#include "json/json.h"
#include "locAware.h"
#include "msgEnum.h"

using std::cout;
using std::endl;
using std::fixed;
using std::ifstream;
using std::ofstream;
using std::setprecision;
using std::showpoint;
using std::string;
using std::stringstream;
using std::vector;

#define coordinationVehicleType 20
#define emergencyVehicleType 2
#define coordinationLaneID 1
#define Minimum_ETA 1.0
#define ETA_Delete_Time 1.0
#define TIME_GAP_BETWEEN_ETA_Update 1
#define SEQUENCE_NUMBER_MINLIMIT 1
#define SEQUENCE_NUMBER_MAXLIMIT 127
#define HOUR_DAY_CONVERSION 24
#define MINTUTE_HOUR_CONVERSION 60
#define SECOND_MINTUTE_CONVERSION 60.0
#define SECOND_MILISECOND_CONVERSION 1000.0
#define Maximum_Number_Of_Priority_Request 15
#define ALLOWED_SPEED_DEVIATION 4.0
#define ALLOWED_ETA_DIFFERENCE 6.0
#define SRM_TIME_GAP_VALUE 8.0
#define ART_UPDATE_FREQUENCY 0.5
#define FeetToMeterConversion 0.3048
#define StandardQueueLengthPerVehicle 25.0

enum msgType
{
    coordinationRequest = 1,
    connectedPassengerVehicleData = 2,
    estimatedArrivalTable = 3
    
};

class PriorityRequestServer
{
private:
    vector<ActiveRequest> ActiveRequestTable;
    int regionalID{};
    int intersectionID{};
    int sequenceNumber{};
    int updateCount{};
    int vehicleType{};
    int vehicleRequestedSignalGroup{};
    int priorityRequestStatus{};
    int requestTimedOutVehicleID{};
    int tempLastTimeETAUpdated{};
    int msgReceived{};
    int msgServed{};
    int msgRejected{};
    double msgSentTime{};
    double arrivalTableSendingTimeToSolver{};
    double arrivalTableReceivedTime{};
    double expectedTimeOfArrivalToStopBar{};
    double requestTimedOutValue{};
    double etaUpdateTime{};
    double vehicleETA{};
    double systemPerformanceSendingTimeInterval{};
    double optimizationProblemSolvingTimegap{2.0};
    double cellLength{};
    bool logging{false};
    bool consoleOutput{false};
    bool emergencyVehicleStatus{false};
    bool sentClearRequest{};
    bool sentClearRequestForEV{};
    bool sendSSM{false};
    bool sendPriorityRequestList{false};
    string intersectionName{};
    string mapPayloadFileName{};
    string optimizationMethod{};
    ofstream logFile;
    /* plocAwareLib is a pointer that points to a variable of the type LocAware. This variable is be created in the constructor of this class, as it requires some other parameters that are available in the constructor.*/
    LocAware *plocAwareLib;
    vector<ArrivalTable> ConnectedPassengerVehicleArrivalTable{};
    vector<ArrivalTable> VehicleArrivalTable{};

public:
    PriorityRequestServer();
    ~PriorityRequestServer();

    string createSSMJsonString(SignalStatus signalStatus);
    string createJsonStringForPrioritySolver();
    string createJsonStringForSystemPerformanceDataLog();
    void manageSignalRequestTable(SignalRequest signalRequest);
    void manageCoordinationRequest(string jsonString);
    void deleteTimedOutRequestfromActiveRequestTable();
    void updateETAInActiveRequestTable();
    void printActiveRequestTable();
    void setRequestTimedOutVehicleID(int timedOutVehicleID);
    void setPriorityRequestStatus();
    void setPRSUpdateCount();
    void setVehicleType(SignalRequest signalRequest);
    void setRequestedSignalGroup(SignalRequest signalRequest);
    void setSrmMessageStatus(SignalRequest signalRequest);
    void setETAUpdateTime();
    void readconfigFile();
    void loggingData(string logString);
    void displayConsoleData(string consoleString);
    void calculateETA(int ETA_Minute, int ETA_Second);
    int getMessageType(string jsonString);
    int getRequestTimedOutVehicleID();
    int getMinuteOfYear();
    int getMsOfMinute();
    int getPRSSequenceNumber();
    int getPRSUpdateCount();
    int getSplitPhase(int signalGroup);
    bool acceptSignalRequest(SignalRequest signalRequest);
    bool addToActiveRequestTable(SignalRequest signalRequest);
    bool updateActiveRequestTable(SignalRequest signalRequest);
    bool deleteRequestfromActiveRequestTable(SignalRequest signalRequest);
    bool checkTimedOutRequestDeletingRequirement();
    bool checkSsmSendingRequirement();
    bool getPriorityRequestListSendingRequirement();
    bool updateETA();
    bool sendClearRequest();
    bool findEVInList();
    bool findEVInRequest(SignalRequest signalRequest);
    bool sendSystemPerformanceDataLog();
    void manageConnectedPassengerVehicleArrivalTable(string jsonString);
    void manageVehicleArrivalTable(string jsonString);
    void updateETAInArrivalTable();
    string getVehicleArrivalTableJsonString();
    bool checkArrivalTableSendingRequirement();
    string getOptimizationMethod();
};