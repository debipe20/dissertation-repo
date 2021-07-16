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
	readInterIntersectionInformationConfig();
	createDataPointStructure();
	msgSendingTime = getPosixTimestamp();

	logFile.open("vehicle-status-prediction-data.txt");

	logFile << "TimeStamp"
			<< ","
			<< "NoOfCells"
			<< ","
			<< "InputVehicleId"
			<< ","
			<< "InputVehicleType"
			<< ","
			<< "InputSignalGroup"
			<< ","
			<< "InputLaneId"
			<< ","
			<< "InputApproachId"
			<< ","
			<< "InputLocationOnMap"
			<< ","
			<< "InputPhaseStatus"
			<< ","
			<< "InputSpeed"
			<< ","
			<< "InputHeading"
			<< ","
			<< "InputDistanceToStopBar"
			<< ","
			<< "InputStoppedDelay"
			<< ","
			<< "InputCellStatus"
			<< ","
			<< "OutputVehicleId"
			<< ","
			<< "OutputVehicleType"
			<< ","
			<< "OutputSignalGroup"
			<< ","
			<< "OutputLaneId"
			<< ","
			<< "OutputApproachId"
			<< ","
			<< "OutputLocationOnMap"
			<< ","
			<< "OutputPhaseStatus"
			<< ","
			<< "OutputSpeed"
			<< ","
			<< "OutputHeading"
			<< ","
			<< "OutputDistanceToStopBar"
			<< ","
			<< "OutputStoppedDelay"
			<< ","
			<< "OutputCellStatus"
			<< endl;
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

void VehicleStatusPredictionDataCollector::readInterIntersectionInformationConfig()
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
		msgSendingFrequency = jsonObject["FrequencyOfRequest"].asDouble();
		penetrationRate = jsonObject["CoonectedVehiclePenetrationRate"].asDouble();
		approachId = jsonObject["ApproachId"].asInt();
		noOfThroughLanes = jsonObject["NoOfThroughLanes"].asInt();
		throughLanesLength = jsonObject["ThroughLanesLength"].asInt();
		noOfLeftTurnPockets = jsonObject["NoOfLeftTurnPocket"].asInt();
		leftTurnPocketsLength = jsonObject["LeftTurnPocketLength"].asInt();
		noOfCellsPerThroughLane = static_cast<int>(throughLanesLength / cellLength);
		noOfCellsPerLeftTurnPocket = static_cast<int>(leftTurnPocketsLength / cellLength);
		throughLaneSignalGroup = jsonObject["ThoughLanesSignalGroup"].asInt();
		leftTurnPocketSignalGroup = jsonObject["LeftTurnPocketsSignalGroup"].asInt();
		totalNoOfCells = (noOfCellsPerLeftTurnPocket * noOfLeftTurnPockets) + (noOfCellsPerThroughLane * noOfThroughLanes);

		for (int i = 0; i < noOfThroughLanes; i++)
			throughLanesId.push_back(jsonObject["ThroughLanesId"][i].asInt());

		for (int i = 0; i < noOfLeftTurnPockets; i++)
			leftTurnPocketsId.push_back(jsonObject["LeftTurnPocketsId"][i].asInt());
	}
}

/*
	- The following method creates the data point structure and fill up the static information for all the cells.
*/
void VehicleStatusPredictionDataCollector::createDataPointStructure()
{
	double cellStartPoint{};
	DataPointStructure dataPointStructure;
	dataPointStructure.reset();

	for (int i = 0; i < noOfLeftTurnPockets; i++)
	{
		for (int j = 0; j < noOfCellsPerLeftTurnPocket; j++)
		{
			dataPointStructure.signalGroup = leftTurnPocketSignalGroup;
			dataPointStructure.laneId = leftTurnPocketsId.at(i);
			dataPointStructure.approachId = approachId;
			dataPointStructure.locationOnMap = static_cast<int>(MsgEnum::mapLocType::onInbound);
			dataPointStructure.cellStartPonit = cellStartPoint;
			dataPointStructure.cellEndPont = cellStartPoint + cellLength;
			dataPointStructure.cellStatus = false;
			dataPointStructure.speed = -1;

			DataPointList.push_back(dataPointStructure);
			cellStartPoint = cellStartPoint + cellLength;
		}
	}

	for (int i = 0; i < noOfThroughLanes; i++)
	{
		cellStartPoint = 0.0;

		for (int j = 0; j < noOfCellsPerThroughLane; j++)
		{
			dataPointStructure.signalGroup = throughLaneSignalGroup;
			dataPointStructure.laneId = throughLanesId.at(i);
			dataPointStructure.approachId = approachId;
			dataPointStructure.locationOnMap = static_cast<int>(MsgEnum::mapLocType::onInbound);
			dataPointStructure.cellStartPonit = cellStartPoint;
			dataPointStructure.cellEndPont = cellStartPoint + cellLength;
			dataPointStructure.cellStatus = false;
			dataPointStructure.speed = -1;

			DataPointList.push_back(dataPointStructure);
			cellStartPoint = cellStartPoint + cellLength;
		}
	}
}

/*
	- The following method can update phase status of all the cells based on the received phase status message
*/
void VehicleStatusPredictionDataCollector::updatePhaseStatusInDataPointList(string jsonString)
{
	int temporarySignalGroup{};
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonObject, &errors);
	delete reader;

	for (int i = 0; i < 8; i++)
	{
		temporarySignalGroup = i + 1;

		if (temporarySignalGroup == leftTurnPocketSignalGroup)
		{
			if (jsonObject["States"][i].asString() == "green")
				leftTurnPocketPhaseStatus = green;

			else if (jsonObject["States"][i].asString() == "yellow")
				leftTurnPocketPhaseStatus = yellow;

			else if (jsonObject["States"][i].asString() == "permissive_yellow")
				leftTurnPocketPhaseStatus = permissive_yellow;

			else if (jsonObject["States"][i].asString() == "red")
				leftTurnPocketPhaseStatus = red;
		}

		else if (temporarySignalGroup == throughLaneSignalGroup)
		{
			if (jsonObject["States"][i].asString() == "green")
				throughLanePhaseStatus = green;

			else if (jsonObject["States"][i].asString() == "yellow")
				throughLanePhaseStatus = yellow;

			else if (jsonObject["States"][i].asString() == "permissive_yellow")
				throughLanePhaseStatus = permissive_yellow;

			else if (jsonObject["States"][i].asString() == "red")
				throughLanePhaseStatus = red;
		}
	}

	for (size_t i = 0; i < DataPointList.size(); i++)
	{
		if (DataPointList[i].signalGroup == leftTurnPocketSignalGroup)
			DataPointList[i].phaseStatus = leftTurnPocketPhaseStatus;

		else if (DataPointList[i].signalGroup == throughLaneSignalGroup)
			DataPointList[i].phaseStatus = throughLanePhaseStatus;
	}
}

/*
	- The following method can fill up the vehicle status information based on the receid vehicle status list
*/
void VehicleStatusPredictionDataCollector::fillUpDataPointList(string jsonString)
{
	int temporaryVehicleID{};
	int temporaryVehicleType{};
	int temporaryLaneId{};
	int temporaryApproachId{};
	double temporarySpeed{};
	double temporaryHeading{};
	double temporaryDistanceToStopBar{};
	double temporaryStoppedDelay{};

	InputDataPointList = DataPointList;
	OutputDataPointList = DataPointList;

	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonObject, &errors);
	delete reader;

	int noOfVehicles = jsonObject["NoOfVehicle"].asInt();
	int noOfInputVehicles = static_cast<int>(noOfVehicles * penetrationRate);

	const Json::Value values = jsonObject["VehcileStatusList"];

	if (noOfVehicles > 0)
	{
		for (int i = 0; i < noOfVehicles; i++)
		{
			for (size_t j = 0; j < values[i].getMemberNames().size(); j++)
			{
				if (values[i].getMemberNames()[j] == "vehicleId")
					temporaryVehicleID = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "vehicleType")
					temporaryVehicleType = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "inBoundLaneId")
					temporaryLaneId = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "inBoundApproachId")
					temporaryApproachId = values[i][values[i].getMemberNames()[j]].asInt();

				else if (values[i].getMemberNames()[j] == "speed_MeterPerSecond")
					temporarySpeed = values[i][values[i].getMemberNames()[j]].asDouble();

				else if (values[i].getMemberNames()[j] == "heading_Degree")
					temporaryHeading = values[i][values[i].getMemberNames()[j]].asDouble();

				else if (values[i].getMemberNames()[j] == "distanceFromStopBar")
					temporaryDistanceToStopBar = values[i][values[i].getMemberNames()[j]].asDouble();

				else if (values[i].getMemberNames()[j] == "stoppedDelay")
					temporaryStoppedDelay = values[i][values[i].getMemberNames()[j]].asDouble();
			}

			if (i < noOfInputVehicles && temporaryApproachId == approachId)
			{
				for (size_t k = 0; k < InputDataPointList.size(); k++)
				{
					if ((temporaryDistanceToStopBar >= InputDataPointList[k].cellStartPonit) &&
						(temporaryDistanceToStopBar <= InputDataPointList[k].cellEndPont) && (temporaryLaneId == InputDataPointList[k].laneId))
					{
						InputDataPointList[k].vehicleID = temporaryVehicleID;
						InputDataPointList[k].vehicleType = temporaryVehicleType;
						InputDataPointList[k].speed = temporarySpeed;
						InputDataPointList[k].heading = temporaryHeading;
						InputDataPointList[k].distanceToStopBar = temporaryDistanceToStopBar;
						InputDataPointList[k].stoppedDelay = temporaryStoppedDelay;
						InputDataPointList[k].cellStatus = true;
					}
				}
			}

			if (temporaryApproachId == approachId)
			{
				for (size_t k = 0; k < OutputDataPointList.size(); k++)
				{
					if ((temporaryDistanceToStopBar >= OutputDataPointList[k].cellStartPonit) &&
						(temporaryDistanceToStopBar <= OutputDataPointList[k].cellEndPont) && (temporaryLaneId == OutputDataPointList[k].laneId))
					{
						OutputDataPointList[k].vehicleID = temporaryVehicleID;
						OutputDataPointList[k].vehicleType = temporaryVehicleType;
						OutputDataPointList[k].speed = temporarySpeed;
						OutputDataPointList[k].heading = temporaryHeading;
						OutputDataPointList[k].distanceToStopBar = temporaryDistanceToStopBar;
						OutputDataPointList[k].stoppedDelay = temporaryStoppedDelay;
						OutputDataPointList[k].cellStatus = true;
					}
				}
			}
		}

		writeCsvFile();
	}
}

void VehicleStatusPredictionDataCollector::writeCsvFile()
{
	double timeStamp = getPosixTimestamp();

	for (size_t i = 0; i < DataPointList.size(); i++)
	{
		logFile << fixed << showpoint << setprecision(4) << timeStamp << "," << totalNoOfCells << ",";
		logFile << fixed << showpoint << setprecision(2) << InputDataPointList[i].vehicleID << "," << InputDataPointList[i].vehicleType
				<< "," << InputDataPointList[i].signalGroup << "," << InputDataPointList[i].laneId << "," << InputDataPointList[i].approachId
				<< "," << InputDataPointList[i].locationOnMap << "," << InputDataPointList[i].phaseStatus << "," << InputDataPointList[i].speed
				<< "," << InputDataPointList[i].heading << "," << InputDataPointList[i].distanceToStopBar << "," << InputDataPointList[i].stoppedDelay
				<< "," << InputDataPointList[i].cellStatus << ",";

		logFile << fixed << showpoint << setprecision(2) << OutputDataPointList[i].vehicleID << "," << OutputDataPointList[i].vehicleType
				<< "," << OutputDataPointList[i].signalGroup << "," << OutputDataPointList[i].laneId << "," << OutputDataPointList[i].approachId
				<< "," << OutputDataPointList[i].locationOnMap << "," << OutputDataPointList[i].phaseStatus << "," << OutputDataPointList[i].speed
				<< "," << OutputDataPointList[i].heading << "," << OutputDataPointList[i].distanceToStopBar << "," << OutputDataPointList[i].stoppedDelay
				<< "," << OutputDataPointList[i].cellStatus << endl;
	}
}

/*
	- The following method checks the vehicle status list message sending requirement.
*/
bool VehicleStatusPredictionDataCollector::checkVehicleStatusListMessageSendingRequirement()
{
	bool messageSendrequirement{false};
	double currentTime = getPosixTimestamp();

	if ((currentTime - msgSendingTime) >= msgSendingFrequency)
		messageSendrequirement = true;

	return messageSendrequirement;
}

/*
	- The following method formulates the vehicle status list request message.
*/
string VehicleStatusPredictionDataCollector::getVehicleStatusListRequestMessage()
{
	string vehicleStatusRequestJsonString{};
	Json::Value jsonObject;
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "";

	jsonObject["MsgType"] = "VehicleStatusListRequest";
	jsonObject["ApproachId"] = approachId;

	vehicleStatusRequestJsonString = Json::writeString(builder, jsonObject);
	msgSendingTime = getPosixTimestamp();

	return vehicleStatusRequestJsonString;
}

VehicleStatusPredictionDataCollector::~VehicleStatusPredictionDataCollector()
{
	logFile.close();
}