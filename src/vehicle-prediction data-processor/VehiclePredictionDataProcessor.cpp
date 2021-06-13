/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    VehiclePredictionDataProcessor.cpp
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
#include "VehiclePredictionDataProcessor.h"
#include "Timestamp.h"


using namespace GeoUtils;
using namespace MsgEnum;

VehiclePredictionDataProcessor::VehiclePredictionDataProcessor(string configFilename)
{
	bool singleFrame{false}; /// TRUE to encode speed limit in lane, FALSE to encode in approach
	fmap = readIntersectionMapConfig(configFilename);

	LocAware *tempPlocAwareLib = new LocAware(fmap, singleFrame);
	plocAwareLib = tempPlocAwareLib;

	createDataPointStructure();
}


/*
	-Get the message type based on the received json string 
*/
int VehiclePredictionDataProcessor::getMessageType(string jsonString)
{
	int messageType{};
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};

	bool parsingSuccessful = reader->parse(jsonString.c_str(), jsonString.c_str() + jsonString.size(), &jsonObject, &errors);
	delete reader;

	if (parsingSuccessful && (jsonObject["MsgType"].asString() == "BSM"))
		messageType = MsgEnum::DSRCmsgID_bsm;
	
	return messageType;
}

string VehiclePredictionDataProcessor::readIntersectionMapConfig(string configFilename)
{
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	ifstream jsonconfigfile(configFilename);
	string mapPayload{};
	string configJsonString((std::istreambuf_iterator<char>(jsonconfigfile)), std::istreambuf_iterator<char>());

	bool parsingSuccessful = reader->parse(configJsonString.c_str(), configJsonString.c_str() + configJsonString.size(), &jsonObject, &errors);
	delete reader;

	if (parsingSuccessful)
	{
		intersectionName = (jsonObject["IntersectionName"]).asString();
		mapPayload = (jsonObject["MapPayload"]).asString();
		//Set the intersection ID
		intersectionId = static_cast<uint16_t>(jsonObject["IntersectionID"].asInt());
		//set the regional ID
		regionalId = static_cast<uint16_t>(jsonObject["RegionalID"].asInt());
	}

	const char *path = "/nojournal/bin";
	stringstream pathStringStream{};
	pathStringStream << path;
	string pathString{};
	pathStringStream >> pathString;
	ofstream outputfile;

	fmap = pathString + "/" + intersectionName + ".map.payload";

	outputfile.open(fmap);
	outputfile << "payload"
			   << " " << intersectionName << " " << mapPayload << std::endl;
	outputfile.close();

	logFile.open("test-data.csv");
	logFile << "TimeStamp" << "," << "VehicleId" << "," << "VehicleType" << "," << "DistanceToStopBar" << "," << "Speed" << "," << "Heading" << "," << "LocationOnMap" << "," << "LaneId" << "," << "ApproachId" << "," << "SignalGroup" << endl;  

	return fmap;
}

void VehiclePredictionDataProcessor::getVehicleInformationFromMAP(BasicVehicle basicVehicle)
{
    string locationOnMap{};
	int vehicleLaneId{};
	int vehicleApproachId{};
	int vehicleSignalGroup{};
	double vehicleDistanceFromStopBar{};
	double timeStamp = getPosixTimestamp();
	bool onMap{false};

	//get the vehicle data from BSM
	double vehicleLatitude = basicVehicle.getLatitude_DecimalDegree();
	double vehicleLongitude = basicVehicle.getLongitude_DecimalDegree();
	double vehicleElevation = basicVehicle.getElevation_Meter();
	double vehicleSpeed = basicVehicle.getSpeed_MeterPerSecond();
	double vehicleHeading = basicVehicle.getHeading_Degree();
	int vehicleId = basicVehicle.getTemporaryID();
	string vehicleType = basicVehicle.getType();

	//initialize all the struct required for MapEngine Library.
	struct geoPoint_t geoPoint_t_1 = {vehicleLatitude, vehicleLongitude, vehicleElevation};
	struct motion_t motion_t_1 = {vehicleSpeed, vehicleHeading};
	struct intersectionTracking_t intersectionTracking_t_1 = {mapLocType::onInbound, 0, 0, 0};
	struct point2D_t point2D_t_1 = {0, 0};
	struct point2D_t point2D_t_2 = {0, 0};
	struct projection_t projection_t_1 = {0.0, 0.0, 0.0};
	struct laneProjection_t laneProjection_t_1 = {0, projection_t_1};
	struct vehicleTracking_t vehicleTracking_t_1 = {intersectionTracking_t_1, laneProjection_t_1};
	std::bitset<4> maneuvers;
	struct dist2go_t dist2go_t_1 = {0.0, 0.0};
	struct connectTo_t connectTo_t_1 = {0, 0, 0, maneuverType::straightAhead};
	std::vector<connectTo_t> connect2go1;
	connect2go1.push_back(connectTo_t_1);
	struct locationAware_t locationAware_t_1 = {0, 0, 0, 0, 0.0, maneuvers, dist2go_t_1, connect2go1};
	struct signalAware_t signalAware_t_1 = {phaseColor::dark, phaseState::redLight, unknown_timeDetail, unknown_timeDetail, unknown_timeDetail};
	struct connectedVehicle_t connectedVehicle_t_1 = {0, 0, 0, geoPoint_t_1, motion_t_1, vehicleTracking_t_1, locationAware_t_1, signalAware_t_1};

	onMap = plocAwareLib->locateVehicleInMap(connectedVehicle_t_1, vehicleTracking_t_1);

	if (onMap)
	{
		plocAwareLib->getPtDist2D(vehicleTracking_t_1, point2D_t_2);
		vehicleDistanceFromStopBar = unsigned(point2D_t_1.distance2pt(point2D_t_2)) / 100; //unit of meters

		if (unsigned(vehicleTracking_t_1.intsectionTrackingState.vehicleIntersectionStatus) == static_cast<int>(MsgEnum::mapLocType::onInbound))
		{
			locationOnMap = "inbound";
			
			vehicleLaneId = plocAwareLib->getLaneIdByIndexes(unsigned(vehicleTracking_t_1.intsectionTrackingState.intersectionIndex),
																 unsigned(vehicleTracking_t_1.intsectionTrackingState.approachIndex),
																 unsigned(vehicleTracking_t_1.intsectionTrackingState.laneIndex));

			vehicleApproachId = plocAwareLib->getApproachIdByLaneId(regionalId, intersectionId, (unsigned char)((unsigned)vehicleLaneId));

			vehicleSignalGroup = unsigned(plocAwareLib->getControlPhaseByIds(static_cast<uint16_t>(regionalId), static_cast<uint16_t>(intersectionId),
																				 static_cast<uint8_t>(vehicleApproachId), static_cast<uint8_t>(vehicleLaneId)));
		}
		else if (unsigned(vehicleTracking_t_1.intsectionTrackingState.vehicleIntersectionStatus) == static_cast<int>(MsgEnum::mapLocType::onOutbound))
			locationOnMap = "outBound";

		else if (unsigned(vehicleTracking_t_1.intsectionTrackingState.vehicleIntersectionStatus) == static_cast<int>(MsgEnum::mapLocType::insideIntersectionBox))
			locationOnMap = "insideIntersectionBox";

		logFile << fixed << showpoint << setprecision(4) << timeStamp << ",";
		logFile << fixed << showpoint << setprecision(2) << vehicleId << "," << vehicleType << "," << vehicleDistanceFromStopBar << "," << vehicleSpeed << "," << vehicleHeading << "," << locationOnMap << "," << vehicleLaneId << "," << vehicleApproachId << "," << vehicleSignalGroup << endl;

	}
}

void VehiclePredictionDataProcessor::createDataPointStructure()
{
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	ifstream jsonconfigfile("mmitss-intersection-information-config.json");
	string configJsonString((std::istreambuf_iterator<char>(jsonconfigfile)), std::istreambuf_iterator<char>());

	bool parsingSuccessful = reader->parse(configJsonString.c_str(), configJsonString.c_str() + configJsonString.size(), &jsonObject, &errors);
	delete reader;
	DataPointStructure dataPointStructure;
	dataPointStructure.reset();
	double cellLength = StandardQueueLengthPerVehicle * FeetToMeterConversion;
	double cellStartPoint{};
	
	if (parsingSuccessful)
	{
		int approachId = jsonObject["ApproachId"].asInt();
		int noOfThroughLanes = jsonObject["NoOfThroughLanes"].asInt();
		int throughLanesLength = jsonObject["ThroughLanesLength"].asInt();
		int noOfLeftTurnPockets = jsonObject["NoOfLeftTurnPocket"].asInt();
		int leftTurnPocketsLength = jsonObject["LeftTurnPocketLength"].asInt();
		int noOfCellsPerThroughLane = static_cast<int>(throughLanesLength / cellLength);
        int noOfCellsPerLeftTurnPocket = static_cast<int>(leftTurnPocketsLength / cellLength);

		for (int i = 0; i < noOfLeftTurnPockets; i++)
		{
			for (int j = 0; j < noOfCellsPerLeftTurnPocket; j++)
			{
				dataPointStructure.signalGroup = jsonObject["LeftTurnPocketsSignalGroup"].asInt();
				dataPointStructure.laneId = jsonObject["LeftTurnPocketsId"][i].asInt();
				dataPointStructure.approachId = approachId;
				dataPointStructure.locationOnMap = static_cast<int>(MsgEnum::mapLocType::onInbound);
				dataPointStructure.cellStartPonit = cellStartPoint;
				dataPointStructure.cellEndPont = cellStartPoint + cellLength;
				dataPointStructure.cellStatus = false;
				
				DataPointList.push_back(dataPointStructure);
				cellStartPoint = cellStartPoint + cellLength;
			}
		}

		for (int i = 0; i < noOfThroughLanes; i++)
		{
			cellStartPoint = 0.0;
			
			for (int j = 0; j < noOfCellsPerThroughLane; j++)
			{
				dataPointStructure.signalGroup = jsonObject["ThoughLanesSignalGroup"].asInt();
				dataPointStructure.laneId = jsonObject["ThroughLanesId"][i].asInt();
				dataPointStructure.approachId = approachId;
				dataPointStructure.locationOnMap = static_cast<int>(MsgEnum::mapLocType::onInbound);
				dataPointStructure.cellStartPonit = cellStartPoint;
				dataPointStructure.cellEndPont = cellStartPoint + cellLength;
				dataPointStructure.cellStatus = false;
				
				DataPointList.push_back(dataPointStructure);
				cellStartPoint = cellStartPoint + cellLength;
			}
		}		
	}
}


VehiclePredictionDataProcessor::~VehiclePredictionDataProcessor()
{
	delete plocAwareLib;
	logFile.close();
}