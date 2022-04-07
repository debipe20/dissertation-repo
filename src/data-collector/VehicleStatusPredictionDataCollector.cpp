/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
	   granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

	VehicleStatusPredictionDataCollector.cpp
	Created by: Debashis Das
	University of Arizona
	College of Engineering

	This code was developed under the supervision of Professor Larry Head
	in the Systems and Industrial Engineering Department.

	Description:

	1.

*/

#include <iterator>
#include <algorithm>
#include "AsnJ2735Lib.h"
#include "locAware.h"
#include "geoUtils.h"
#include "msgEnum.h"
#include "VehicleStatusPredictionDataCollector.h"
#include "Timestamp.h"

using namespace GeoUtils;
using namespace MsgEnum;

VehicleStatusPredictionDataCollector::VehicleStatusPredictionDataCollector()
{
	readIntersectionInformationConfig();
	createDataPointStructure(approach1SignalGroup, approach1LaneId, approachId1);
	createDataPointStructure(approach2SignalGroup, approach2LaneId, approachId2);
	// createDataPointStructure(approach3SignalGroup, approach3LaneId, approachId3);
	// createDataPointStructure(approach4SignalGroup, approach4LaneId, approachId4);
	createLogFile(logFileApproach1, approachId1);
	createLogFile(logFileApproach2, approachId2);
	// createLogFile(logFileApproach3, approachId3);
	// createLogFile(logFileApproach4, approachId4);
}

/*
	-Get the message type based on the received json string
*/
int VehicleStatusPredictionDataCollector::getMessageType(string jsonString)
{
	int messageType{};
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};

	bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonObject, &errors);
	delete reader;

	if (parsingSuccessful)
	{
		if (jsonObject["MsgType"].asString() == "CurrentState_VehiclePhases")
			messageType = static_cast<int>(msgType::CurrentState_VehiclePhases);

		else if (jsonObject["MsgType"].asString() == "VehicleStatusList")
			messageType = static_cast<int>(msgType::VehicleStatus);
	}

	return messageType;
}

void VehicleStatusPredictionDataCollector::readIntersectionInformationConfig()
{
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	ifstream jsonconfigfile("mmitss-intersection-information-config.json");
	string configJsonString((std::istreambuf_iterator<char>(jsonconfigfile)), std::istreambuf_iterator<char>());

	bool parsingSuccessful = reader->parse(configJsonString.c_str(), configJsonString.c_str() + configJsonString.size(), &jsonObject, &errors);
	delete reader;
	cellLength = StandardQueueLengthPerVehicle * FeetToMeterConversion;

	if (parsingSuccessful)
	{
		intersectionName = jsonObject["IntersectionName"].asString();
		trainingData = jsonObject["TrainingData"].asBool();
		penetrationRate = jsonObject["CoonectedVehiclePenetrationRate"].asDouble();
		dataStructurewidth = jsonObject["DataStructureWidth"].asInt();
		dataStructureHeight = jsonObject["DataStructureHeight"].asInt();
		approach1ThroughSignalGroup = jsonObject["Approach1ThroughSignalGroup"].asInt();
		approach1LeftTurnSignalGroup = jsonObject["Approach1LeftTurnSignalGroup"].asInt();
		approach2ThroughSignalGroup = jsonObject["Approach2ThroughSignalGroup"].asInt();
		approach2LeftTurnSignalGroup = jsonObject["Approach2LeftTurnSignalGroup"].asInt();
		approach3ThroughSignalGroup = jsonObject["Approach3ThroughSignalGroup"].asInt();
		approach3LeftTurnSignalGroup = jsonObject["Approach3LeftTurnSignalGroup"].asInt();
		approach4ThroughSignalGroup = jsonObject["Approach4ThroughSignalGroup"].asInt();
		approach4LeftTurnSignalGroup = jsonObject["Approach4LeftTurnSignalGroup"].asInt();
		approachId1 = jsonObject["ApproachId"][0].asInt();
		approachId2 = jsonObject["ApproachId"][1].asInt();
		approachId3 = jsonObject["ApproachId"][2].asInt();
		approachId4 = jsonObject["ApproachId"][3].asInt();

		// totalNoOfCells = (noOfCellsPerLeftTurnPocket * noOfLeftTurnPockets) + (noOfCellsPerThroughLane * noOfThroughLanes);

		// Get Lane Id from the config file
		for (int i = 0; i < dataStructurewidth; i++)
			approach1LaneId.push_back(jsonObject["Approach1LaneId"][i].asInt());

		for (int i = 0; i < dataStructurewidth; i++)
			approach2LaneId.push_back(jsonObject["Approach2LaneId"][i].asInt());

		for (int i = 0; i < dataStructurewidth; i++)
			approach3LaneId.push_back(jsonObject["Approach3LaneId"][i].asInt());

		for (int i = 0; i < dataStructurewidth; i++)
			approach4LaneId.push_back(jsonObject["Approach4LaneId"][i].asInt());

		// Get Signal Group from the config file
		for (int i = 0; i < dataStructurewidth; i++)
			approach1SignalGroup.push_back(jsonObject["Approach1SignalGroup"][i].asInt());

		for (int i = 0; i < dataStructurewidth; i++)
			approach2SignalGroup.push_back(jsonObject["Approach2SignalGroup"][i].asInt());

		for (int i = 0; i < dataStructurewidth; i++)
			approach3SignalGroup.push_back(jsonObject["Approach3SignalGroup"][i].asInt());

		for (int i = 0; i < dataStructurewidth; i++)
			approach4SignalGroup.push_back(jsonObject["Approach4SignalGroup"][i].asInt());
	}
}

void VehicleStatusPredictionDataCollector::createLogFile(ofstream& logFile, int approachId)
{
	stringstream stream;
	stream << fixed << setprecision(2) << penetrationRate;
	string penetrationRateString = stream.str();
	if (trainingData)
	{
		logFile.open("data/" + intersectionName + "-approach" + std::to_string(approachId) + "-vehicle-status-data-" + penetrationRateString + ".csv");

		logFile << "TimeStamp"
				<< ","
				<< "NoOfCells"
				<< ","
				<< "VehicleType"
				<< ","
				<< "SignalGroup"
				<< ","
				<< "PhaseStatus"
				<< ","
				<< "PhaseElapsedTime"
				<< ","
				<< "Speed"
				<< ","
				<< "DistanceToStopBar"
				<< ","
				<< "ApproachId"
				<< ","
				<< "LaneId"
				<< ","
				<< "CellStatus"
				<< ","
				<< "OutputSpeed"
				<< endl;
	}

	else
	{
		logFile.open("data/sample-vehicle-status-data-" + penetrationRateString + ".csv");

		logFile << "TimeStamp"
				<< ","
				<< "NoOfCells"
				<< ","
				<< "CellNo"
				<< ","
				<< "NoOfConnectedVehicle"
				<< ","
				<< "NoOfNonConnectedVehicle"
				<< ","
				<< "ConnectedVehicleId"
				<< ","
				<< "NonConnectedVehicleId"
				<< ","
				<< "VehicleType"
				<< ","
				<< "SignalGroup"
				<< ","
				<< "PhaseStatus"
				<< ","
				<< "PhaseElapsedTime"
				<< ","
				<< "Speed"
				<< ","
				<< "DistanceToStopBar"
				<< ","
				<< "ApproachId"
				<< ","
				<< "LaneId"
				<< ","
				<< "CellStatus"
				<< ","
				<< "OutputSpeed"
				<< endl;
	}
}

/*
	- The following method creates the data point structure and fill up the static information for all the cells.
*/
void VehicleStatusPredictionDataCollector::createDataPointStructure(vector<int> signalGroup, vector<int> laneId, int approachId)
{
	double cellStartPoint{};
	int cellNumber{};
	vector<DataPointStructure> dataPointList{};
	DataPointStructure dataPointStructure;
	dataPointStructure.reset();


	for (int i = 0; i < dataStructurewidth; i++)
	{
		cellStartPoint = 0.0;
		cellNumber = 1;

		for (int j = 0; j < dataStructureHeight; j++)
		{
			dataPointStructure.cellNo = cellNumber++;
			dataPointStructure.connectedVehicleID = 0;
			dataPointStructure.nonConnectedVehicleID = 0;
			dataPointStructure.signalGroup = signalGroup[i];
			dataPointStructure.laneId = laneId[i];
			dataPointStructure.approachId = approachId;
			dataPointStructure.locationOnMap = static_cast<int>(MsgEnum::mapLocType::onInbound);
			dataPointStructure.cellStartPonit = cellStartPoint;
			dataPointStructure.cellEndPont = cellStartPoint + cellLength;
			dataPointStructure.distanceToStopBar = cellStartPoint + (cellLength / 2);
			dataPointStructure.speed = -1.0;
			dataPointStructure.cellStatus = false;
			dataPointStructure.outputSpeed = -1.0;

			dataPointList.push_back(dataPointStructure);
			cellStartPoint = cellStartPoint + cellLength;
		}
	}

	if (approachId == approachId1)
		DataPointListApproach1 = dataPointList;
	
	else if (approachId == approachId2)
		DataPointListApproach2 = dataPointList;

	else if (approachId == approachId3)
		DataPointListApproach3 = dataPointList;

	else if (approachId == approachId4)
		DataPointListApproach4 = dataPointList;
	// DataPointList.insert(DataPointList.end(), dataPointList.begin(), dataPointList.end());
}

/*
	- The following method can update phase status of all the cells based on the received phase status message
*/
void VehicleStatusPredictionDataCollector::updatePhaseStatusInDataPointList(vector<DataPointStructure> dataPointList, string jsonString, int throughSignalGroup, int leftTurnSignalGroup)
{
	vector<int> temporarySignalGroup{};
	vector<int> temporarySignalGroupStates{};
	// int temporaryPhaseState{};
	double currentTime = getPosixTimestamp();
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonObject, &errors);
	delete reader;

	for (int i = 0; i < 8; i++)
	{
		temporarySignalGroup.push_back(i + 1);

		if (jsonObject["States"][i].asString() == "green")
			temporarySignalGroupStates.push_back(green);

		else if (jsonObject["States"][i].asString() == "yellow")
			temporarySignalGroupStates.push_back(yellow);

		else if (jsonObject["States"][i].asString() == "permissive_yellow")
			temporarySignalGroupStates.push_back(permissive_yellow);

		else if (jsonObject["States"][i].asString() == "red")
			temporarySignalGroupStates.push_back(red);
	}

	for (size_t i = 0; i < dataPointList.size(); i++)
	{
		if (dataPointList[i].signalGroup == 0)
			continue;

		else if ((dataPointList[i].signalGroup == leftTurnSignalGroup) && (dataPointList[i].phaseStatus == temporarySignalGroupStates[leftTurnSignalGroup - 1]))
			dataPointList[i].phaseElapsedTime = currentTime - dataPointList[i].phaseUpdateTime;

		else if ((dataPointList[i].signalGroup == leftTurnSignalGroup) && (dataPointList[i].phaseStatus != temporarySignalGroupStates[leftTurnSignalGroup - 1]))
		{
			dataPointList[i].phaseElapsedTime = 0.0;
			dataPointList[i].phaseUpdateTime = currentTime;
			dataPointList[i].phaseStatus = temporarySignalGroupStates[leftTurnSignalGroup - 1];
		}

		else if ((dataPointList[i].signalGroup == throughSignalGroup) && (dataPointList[i].phaseStatus == temporarySignalGroupStates[throughSignalGroup - 1]))
			dataPointList[i].phaseElapsedTime = currentTime - dataPointList[i].phaseUpdateTime;

		else if ((dataPointList[i].signalGroup == throughSignalGroup) && (dataPointList[i].phaseStatus != temporarySignalGroupStates[throughSignalGroup - 1]))
		{
			dataPointList[i].phaseElapsedTime = 0.0;
			dataPointList[i].phaseUpdateTime = currentTime;
			dataPointList[i].phaseStatus = temporarySignalGroupStates[throughSignalGroup - 1];
		}
	}
}

void VehicleStatusPredictionDataCollector::processSpatData(string jsonString)
{
	updatePhaseStatusInDataPointList(InputDataPointListApproach1, jsonString, approach1ThroughSignalGroup, approach1LeftTurnSignalGroup);
	updatePhaseStatusInDataPointList(InputDataPointListApproach2, jsonString, approach2ThroughSignalGroup, approach2LeftTurnSignalGroup);
	// updatePhaseStatusInDataPointList(InputDataPointListApproach3, jsonString, approach3ThroughSignalGroup, approach3LeftTurnSignalGroup);
	// updatePhaseStatusInDataPointList(InputDataPointListApproach4, jsonString, approach4ThroughSignalGroup, approach4LeftTurnSignalGroup);
}
/*
	- The following method can fill up the vehicle status information based on the receid vehicle status list
*/
void VehicleStatusPredictionDataCollector::processVehicleStatusData(string jsonString)
{
	int temporaryVehicleId{};
	int temporaryVehicleType{};
	int temporaryLaneId{};
	int temporaryApproachId{};
	double temporarySpeed{};
	double temporaryDistanceToStopBar{};
	bool temporaryConnectedVehicleStatus{};
	noOfConnectedVehicle = 0;
	noOfNonConnectedVehicle = 0;

	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonObject, &errors);
	delete reader;

	int noOfVehicles = jsonObject["NoOfVehicle"].asInt();
	const Json::Value values = jsonObject["VehicleStatusList"];

	if (noOfVehicles > 0)
	{
		for (int i = 0; i < noOfVehicles; i++)
		{
			for (size_t j = 0; j < values[i].getMemberNames().size(); j++)
			{
				if (values[i].getMemberNames()[j] == "vehicleId")
					temporaryVehicleId = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "vehicleType")
					temporaryVehicleType = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "inBoundLaneId")
					temporaryLaneId = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "inBoundApproachId")
					temporaryApproachId = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "speed_MeterPerSecond")
					temporarySpeed = values[i][values[i].getMemberNames()[j]].asDouble();

				else if (values[i].getMemberNames()[j] == "distanceFromStopBar")
					temporaryDistanceToStopBar = values[i][values[i].getMemberNames()[j]].asDouble();

				else if (values[i].getMemberNames()[j] == "connectedVehicleStatus")
					temporaryConnectedVehicleStatus = values[i][values[i].getMemberNames()[j]].asBool();
			}

			
				fillUpDataPointList(temporaryApproachId, temporaryVehicleId, temporaryVehicleType, temporarySpeed, temporaryDistanceToStopBar, temporaryLaneId, temporaryConnectedVehicleStatus);

			// else if (temporaryApproachId == approachId2)
			// 	fillUpDataPointList(InputDataPointListApproach2, temporaryVehicleId, temporaryVehicleType, temporarySpeed, temporaryDistanceToStopBar, temporaryLaneId, temporaryConnectedVehicleStatus);

			// else if (temporaryApproachId == approachId3)
			// 	fillUpDataPointList(InputDataPointListApproach1, temporaryVehicleId, temporaryVehicleType, temporarySpeed, temporaryDistanceToStopBar, temporaryLaneId, temporaryConnectedVehicleStatus);

			// else if (temporaryApproachId == approachId4)
			// 	fillUpDataPointList(InputDataPointListApproach4, temporaryVehicleId, temporaryVehicleType, temporarySpeed, temporaryDistanceToStopBar, temporaryLaneId, temporaryConnectedVehicleStatus);
		}
	}

	writeCsvFile(logFileApproach1, InputDataPointListApproach1);
	writeCsvFile(logFileApproach2, InputDataPointListApproach2);
	// writeCsvFile(logFileApproach3, InputDataPointListApproach3);
	// writeCsvFile(logFileApproach4, InputDataPointListApproach4);
}

void VehicleStatusPredictionDataCollector::fillUpDataPointList(int approachId, int vehicleId, int vehicleType, double vehicleSpeed,
															   double distanceToStopBar, int laneId, bool connectedVehicleStatus)
{
	vector<DataPointStructure> InputDataPointList{};

	if (approachId == approachId1)
		InputDataPointList = InputDataPointListApproach1;
	
	else if (approachId == approachId2)
		InputDataPointList = InputDataPointListApproach2;

	else if (approachId == approachId3)
		InputDataPointList = InputDataPointListApproach3;

	else if (approachId == approachId4)
		InputDataPointList = InputDataPointListApproach4;

	for (size_t k = 0; k < InputDataPointList.size(); k++)
	{
		if ((distanceToStopBar >= InputDataPointList[k].cellStartPonit) &&
			(distanceToStopBar <= InputDataPointList[k].cellEndPont) && (laneId == InputDataPointList[k].laneId) &&
			(connectedVehicleStatus))
		{
			InputDataPointList[k].connectedVehicleID = vehicleId;
			InputDataPointList[k].vehicleType = vehicleType;
			InputDataPointList[k].speed = vehicleSpeed;
			// InputDataPointList[k].heading = temporaryHeading;
			InputDataPointList[k].cellStatus = true;
			InputDataPointList[k].outputSpeed = vehicleSpeed;

			if (vehicleSpeed >= 0.0 && k > 0)
				noOfConnectedVehicle++;
		}

		else if ((distanceToStopBar >= InputDataPointList[k].cellStartPonit) &&
				 (distanceToStopBar <= InputDataPointList[k].cellEndPont) && (laneId == InputDataPointList[k].laneId) &&
				 (!connectedVehicleStatus))
		{
			InputDataPointList[k].nonConnectedVehicleID = vehicleId;
			InputDataPointList[k].cellStatus = true;
			InputDataPointList[k].outputSpeed = vehicleSpeed;
			noOfNonConnectedVehicle++;
		}
	}

	if (approachId == approachId1)
		InputDataPointListApproach1 = InputDataPointList;
	
	else if (approachId == approachId2)
		InputDataPointListApproach2 = InputDataPointList;

	else if (approachId == approachId3)
		InputDataPointListApproach3 = InputDataPointList;

	else if (approachId == approachId4)
		InputDataPointListApproach4 = InputDataPointList;
}

void VehicleStatusPredictionDataCollector::writeCsvFile(ofstream& logFile, vector<DataPointStructure> InputDataPointList)
{
	double timeStamp = getPosixTimestamp();

	if (trainingData)
	{
		for (size_t i = 0; i < InputDataPointList.size(); i++)
		{
			logFile << fixed << showpoint << setprecision(4) << timeStamp << "," << totalNoOfCells << ",";
			logFile << fixed << showpoint << setprecision(2) << InputDataPointList[i].vehicleType << ","
					<< InputDataPointList[i].signalGroup << "," << InputDataPointList[i].phaseStatus << ","
					<< InputDataPointList[i].phaseElapsedTime << "," << InputDataPointList[i].speed << ","
					<< InputDataPointList[i].distanceToStopBar << "," << InputDataPointList[i].approachId << ","
					<< InputDataPointList[i].laneId << "," << InputDataPointList[i].cellStatus << ","
					<< InputDataPointList[i].outputSpeed << endl;
		}
	}

	else
	{
		for (size_t i = 0; i < InputDataPointList.size(); i++)
		{
			logFile << fixed << showpoint << setprecision(4) << timeStamp << "," << totalNoOfCells << ",";
			logFile << fixed << showpoint << setprecision(2) << InputDataPointList[i].cellNo << ",";
			logFile << fixed << showpoint << setprecision(2) << noOfConnectedVehicle << "," << noOfNonConnectedVehicle << ",";
			logFile << fixed << showpoint << setprecision(2) << InputDataPointList[i].connectedVehicleID << "," << InputDataPointList[i].nonConnectedVehicleID << ","
					<< InputDataPointList[i].vehicleType << "," << InputDataPointList[i].signalGroup << "," << InputDataPointList[i].phaseStatus << ","
					<< InputDataPointList[i].phaseElapsedTime << "," << InputDataPointList[i].speed << ","
					<< InputDataPointList[i].distanceToStopBar << "," << InputDataPointList[i].approachId << ","
					<< InputDataPointList[i].laneId << "," << InputDataPointList[i].cellStatus << ","
					<< InputDataPointList[i].outputSpeed << endl;
		}
	}
}

/*
	- The following method checks the vehicle status list message sending requirement.
*/
// bool VehicleStatusPredictionDataCollector::checkVehicleStatusListMessageSendingRequirement()
// {
// 	bool messageSendrequirement{false};
// 	double currentTime = getPosixTimestamp();

// 	if ((currentTime - msgSendingTime) >= msgSendingFrequency)
// 		messageSendrequirement = true;

// 	return messageSendrequirement;
// }

/*
	- The following method formulates the vehicle status list request message.
*/
// string VehicleStatusPredictionDataCollector::getVehicleStatusListRequestMessage()
// {
// 	string vehicleStatusRequestJsonString{};
// 	Json::Value jsonObject;
// 	Json::StreamWriterBuilder builder;
// 	builder["commentStyle"] = "None";
// 	builder["indentation"] = "";

// 	jsonObject["MsgType"] = "VehicleStatusListRequest";
// 	jsonObject["ApproachId"] = approachId;

// 	vehicleStatusRequestJsonString = Json::writeString(builder, jsonObject);
// 	msgSendingTime = getPosixTimestamp();

// 	return vehicleStatusRequestJsonString;
// }

// vector<int> VehicleStatusPredictionDataCollector::generateRandomNumber(int noOfVehicle, int noOfInputVehicle)
// {
// 	int randomNumber{};
// 	int lower = 0;
// 	int upper = noOfVehicle;
// 	int count = noOfInputVehicle;
// 	vector<int> randomIndex{};
// 	vector<int>::iterator it;

// 	for (int i = 0; i < count; i++)
// 	{
// 		randomNumber = (rand() % (upper - lower + 1)) + lower;
// 		cout << randomNumber << endl;

// 		it = std::find(randomIndex.begin(), randomIndex.end(), randomNumber);

// 		if (it == randomIndex.end())
// 			randomIndex.push_back(randomNumber);

// 		else
// 			i--;
// 	}
// 	return randomIndex;
// }

VehicleStatusPredictionDataCollector::~VehicleStatusPredictionDataCollector()
{
	logFileApproach1.close();
	logFileApproach2.close();
	// logFileApproach3.close();
	// logFileApproach4.close();
}