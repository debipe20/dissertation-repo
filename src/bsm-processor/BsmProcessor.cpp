/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    BsmProcessor.cpp
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
#include "BsmProcessor.h"
#include "Timestamp.h"


using namespace GeoUtils;
using namespace MsgEnum;

BsmProcessor::BsmProcessor(string configFilename)
{
	bool singleFrame{false}; /// TRUE to encode speed limit in lane, FALSE to encode in approach
	fmap = readIntersectionMapConfig(configFilename);

	LocAware *tempPlocAwareLib = new LocAware(fmap, singleFrame);
	plocAwareLib = tempPlocAwareLib;
}


/*
	-Get the message type based on the received json string 
*/
int BsmProcessor::getMessageType(string jsonString)
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

string BsmProcessor::readIntersectionMapConfig(string configFilename)
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

	logFile.open("data.csv");
	logFile << "TimeStamp" << "," << "VehicleId" << "," << "VehicleType" << "," << "DistanceToStopBar" << "," << "Speed" << "," << "Heading" << "," << "LocationOnMap" << "," << "LaneId" << "," << "ApproachId" << "," << "SignalGroup" << endl;  

	return fmap;
}

void BsmProcessor::getVehicleInformationFromMAP(BasicVehicle basicVehicle)
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

		logFile << fixed << showpoint << setprecision(6) << timeStamp << "," << vehicleId << "," << vehicleType << "," << vehicleDistanceFromStopBar << "," << vehicleSpeed << "," << vehicleHeading << "," << vehicleLaneId << "," << vehicleApproachId << "," << vehicleSignalGroup << endl;

	}
}

BsmProcessor::~BsmProcessor()
{
	delete plocAwareLib;
	logFile.close();
}