/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    VehicleStatusManager.cpp
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
#include "VehicleStatusManager.h"
#include "Timestamp.h"

using namespace GeoUtils;
using namespace MsgEnum;

VehicleStatusManager::VehicleStatusManager(string configFilename)
{
	bool singleFrame{false}; /// TRUE to encode speed limit in lane, FALSE to encode in approach
	fmap = readIntersectionMapConfig(configFilename);
	readConfigFile();

	LocAware *tempPlocAwareLib = new LocAware(fmap, singleFrame);
	plocAwareLib = tempPlocAwareLib;
}

void VehicleStatusManager::readConfigFile()
{
	Json::Value jsonObject;
	Json::CharReaderBuilder builder;
	Json::CharReader *reader = builder.newCharReader();
	string errors{};
	ifstream jsonconfigfile("mmitss-vehicle-status-manager-config.json");
	string configJsonString((std::istreambuf_iterator<char>(jsonconfigfile)), std::istreambuf_iterator<char>());

	bool parsingSuccessful = reader->parse(configJsonString.c_str(), configJsonString.c_str() + configJsonString.size(), &jsonObject, &errors);
	delete reader;

	if (parsingSuccessful)
	{
		penetrationRate = jsonObject["ConnectedVehiclePenetrationRate"].asDouble();
		msgSendingFrequency = jsonObject["FrequencyOfRequest"].asDouble();
		approachId = jsonObject["ApproachId"].asInt();
		noOfLanes = jsonObject["NoOfLanes"].asInt();

		for (int i = 0; i < noOfLanes; i++)
			lanesId.push_back(jsonObject["LaneId"][i].asInt());
	}
}

string VehicleStatusManager::readIntersectionMapConfig(string configFilename)
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
			   << " " << intersectionName << " " << mapPayload << endl;
	outputfile.close();

	return fmap;
}

void VehicleStatusManager::getVehicleInformationFromMAP(BasicVehicle basicVehicle)
{
	int vehicleId{};
	double currentTime{};
	bool onMap{false};
	vehicleId = basicVehicle.getTemporaryID();

	vector<VehicleStatus>::iterator findVehicleIdInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																					[&](VehicleStatus const &p)
																					{ return p.vehicleId == vehicleId; });

	//get the vehicle data from BSM
	double vehicle_Latitude = basicVehicle.getLatitude_DecimalDegree();
	double vehicle_Longitude = basicVehicle.getLongitude_DecimalDegree();
	double vehicle_Elevation = basicVehicle.getElevation_Meter();
	double vehicle_Speed = basicVehicle.getSpeed_MeterPerSecond();
	double vehicle_Heading = basicVehicle.getHeading_Degree();

	//initialize all the struct required for MapEngine Library.
	struct geoPoint_t geoPoint_t_1 = {vehicle_Latitude, vehicle_Longitude, vehicle_Elevation};
	struct motion_t motion_t_1 = {vehicle_Speed, vehicle_Heading};
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
		double vehicleDistanceFromStopBar = unsigned(point2D_t_1.distance2pt(point2D_t_2)) / 100; //unit of meters

		int vehicleLaneId = plocAwareLib->getLaneIdByIndexes(unsigned(vehicleTracking_t_1.intsectionTrackingState.intersectionIndex),
															 unsigned(vehicleTracking_t_1.intsectionTrackingState.approachIndex),
															 unsigned(vehicleTracking_t_1.intsectionTrackingState.laneIndex));

		int vehicleApproachId = plocAwareLib->getApproachIdByLaneId(regionalId, intersectionId, (unsigned char)((unsigned)vehicleLaneId));

		int vehicleSignalGroup = unsigned(plocAwareLib->getControlPhaseByIds(static_cast<uint16_t>(regionalId), static_cast<uint16_t>(intersectionId),
																			 static_cast<uint8_t>(vehicleApproachId), static_cast<uint8_t>(vehicleLaneId)));

		findVehicleIdInVehicleStatusList->vehicleDistanceFromStopBar = vehicleDistanceFromStopBar;
		findVehicleIdInVehicleStatusList->vehicleLaneId = vehicleLaneId;
		findVehicleIdInVehicleStatusList->vehicleApproachId = vehicleApproachId;
		findVehicleIdInVehicleStatusList->vehicleSignalGroup = vehicleSignalGroup;
		findVehicleIdInVehicleStatusList->vehicleLocationOnMap = unsigned(vehicleTracking_t_1.intsectionTrackingState.vehicleIntersectionStatus);

		currentTime = getPosixTimestamp();

		if (vehicle_Speed <= 1.0)
		{
			double vehicleStoppedDelay = findVehicleIdInVehicleStatusList->vehicleStoppedDelay + currentTime - findVehicleIdInVehicleStatusList->updateTime;
			findVehicleIdInVehicleStatusList->vehicleStoppedDelay = vehicleStoppedDelay;
			findVehicleIdInVehicleStatusList->updateTime = currentTime;
		}

		else
		{
			findVehicleIdInVehicleStatusList->updateTime = currentTime;
			findVehicleIdInVehicleStatusList->vehicleStoppedDelay = 0;
		}
	}

	else
	{
		if(findVehicleIdInVehicleStatusList->connected)
			noOfConnectedVehiclesInList--;

		VehicleStatusList.erase(findVehicleIdInVehicleStatusList);
		
	}
}

/*
    - The following method will check whether the received vehicle information is required to add or update  in the VehcileStatus List
    - If the received BSM is from a new vehicle, the method will create a vehicle inforamtion object for the vehicle.
    - If vehicle id of the received BSM is already present in the VehcileStatus List, the method will update the vehicle position and time.
*/
void VehicleStatusManager::manageVehicleStatusList(BasicVehicle basicVehicle)
{
	int vehicleId{};
	int vehicleType{};

	VehicleStatus vehicleStatus;
	vehicleStatus.reset();

	if (addVehicleIDInVehicleStatusList(basicVehicle))
	{
		vehicleId = basicVehicle.getTemporaryID();
		if (basicVehicle.getType() == "Transit")
			vehicleType = Transit;

		else if (basicVehicle.getType() == "Truck")
			vehicleType = Truck;

		else if (basicVehicle.getType() == "EmergencyVehicle")
			vehicleType = EmergencyVehicle;

		else if (basicVehicle.getType() == "Car")
			vehicleType = PassengerVehicle;

		vehicleStatus.vehicleId = vehicleId;
		vehicleStatus.vehicleType = vehicleType;
		vehicleStatus.vehicleSpeed_MeterPerSecond = basicVehicle.getSpeed_MeterPerSecond();
		vehicleStatus.vehicleHeading_Degree = basicVehicle.getHeading_Degree();
		vehicleStatus.updateTime = getPosixTimestamp();

		VehicleStatusList.push_back(vehicleStatus);
		
		
		setConnectedVehicleStatus(vehicleId);
	}

	else if (updateVehicleIDInVehicleStatusList(basicVehicle))
	{
		vehicleId = basicVehicle.getTemporaryID();

		vector<VehicleStatus>::iterator findVehicleIdInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																						[&](VehicleStatus const &p)
																						{ return p.vehicleId == vehicleId; });

		findVehicleIdInVehicleStatusList->vehicleSpeed_MeterPerSecond = basicVehicle.getSpeed_MeterPerSecond();
		findVehicleIdInVehicleStatusList->vehicleHeading_Degree = basicVehicle.getHeading_Degree();
	}

	getVehicleInformationFromMAP(basicVehicle);
	deleteVehicleIDInVehicleStatusList(basicVehicle);
	deleteTimedOutVehicleIdFromVehicleStatusList();

}

void VehicleStatusManager::setConnectedVehicleStatus(int vehicleId)
{
	int noOfVehiclesInList = static_cast<int>(VehicleStatusList.size());
	int allowedConnectedVehicle = static_cast<int>(noOfVehiclesInList * penetrationRate);

	vector<VehicleStatus>::iterator findVehicleIdInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																						[&](VehicleStatus const &p)
																						{ return p.vehicleId == vehicleId; });

	if (noOfVehiclesInList == 1)
	{
		findVehicleIdInVehicleStatusList->connected = true;
		noOfConnectedVehiclesInList++;
	}

	else if (noOfConnectedVehiclesInList <= allowedConnectedVehicle)
	{
		findVehicleIdInVehicleStatusList->connected = true;
		noOfConnectedVehiclesInList++;
	}

	else
		findVehicleIdInVehicleStatusList->connected = false;
}

/*
    - The following boolean method will determine whether the received vehicle information is required to add in the VehcileStatus List
    - If vehicle ID is not present in the VehcileStatus list, the method will return true. 
*/
bool VehicleStatusManager::addVehicleIDInVehicleStatusList(BasicVehicle basicVehicle)
{
	bool addVehicleID{false};
	int vehicleId = basicVehicle.getTemporaryID();

	vector<VehicleStatus>::iterator findVehicleIdInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																					[&](VehicleStatus const &p)
																					{ return p.vehicleId == vehicleId; });

	if (VehicleStatusList.empty())
		addVehicleID = true;

	else if (!VehicleStatusList.empty() && findVehicleIdInVehicleStatusList == VehicleStatusList.end())
		addVehicleID = true;

	return addVehicleID;
}

/*
    - The following boolean method will determine whether the received vehicle information is required to update in the VehcileStatus List
    - If vehicle ID is present in the VehcileStatus list, the method will return true.
*/
bool VehicleStatusManager::updateVehicleIDInVehicleStatusList(BasicVehicle basicVehicle)
{
	bool updateVehicleID{false};
	int veheicleID = basicVehicle.getTemporaryID();

	vector<VehicleStatus>::iterator findVehicleIdInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																					[&](VehicleStatus const &p)
																					{ return p.vehicleId == veheicleID; });

	if (VehicleStatusList.empty())
		updateVehicleID = false;

	else if (!VehicleStatusList.empty() && findVehicleIdInVehicleStatusList != VehicleStatusList.end())
		updateVehicleID = true;

	else if (!VehicleStatusList.empty() && findVehicleIdInVehicleStatusList == VehicleStatusList.end())
		updateVehicleID = false;

	return updateVehicleID;
}

/*
    - The following method will delete vehicle information from the VehcileStatus List if vehicle is not inBound lane
*/
void VehicleStatusManager::deleteVehicleIDInVehicleStatusList(BasicVehicle basicVehicle)
{
	if (!VehicleStatusList.empty())
	{
		int veheicleID = basicVehicle.getTemporaryID();

		vector<VehicleStatus>::iterator findVehicleIdInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																						[&](VehicleStatus const &p)
																						{ return p.vehicleId == veheicleID; });
		if (findVehicleIdInVehicleStatusList != VehicleStatusList.end() && findVehicleIdInVehicleStatusList->vehicleLocationOnMap != 2)
		{
			if(findVehicleIdInVehicleStatusList->connected)
				noOfConnectedVehiclesInList--;

			VehicleStatusList.erase(findVehicleIdInVehicleStatusList);
		}
	}
}
/*
    - The following boolean method will determine whether vehicle information is required to delete from the VehcileStatus List
    - If there BSM is not received from a vehicle for more than predifined time(10sec),the method will delete the timed-out vehilce information form VehcileStatus List.
*/
void VehicleStatusManager::deleteTimedOutVehicleIdFromVehicleStatusList()
{
	double currentTime = static_cast<double>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
	int vehicleId{};

	if (!VehicleStatusList.empty())
	{
		for (size_t i = 0; i < VehicleStatusList.size(); i++)
		{
			if (currentTime - VehicleStatusList[i].updateTime > Vehicle_Timed_Out_Value)
			{
				vehicleId = VehicleStatusList[i].vehicleId;

				vector<VehicleStatus>::iterator findVehicleIDInVehicleStatusList = std::find_if(std::begin(VehicleStatusList), std::end(VehicleStatusList),
																								[&](VehicleStatus const &p)
																								{ return p.vehicleId == vehicleId; });
				if (findVehicleIDInVehicleStatusList->connected)
					noOfConnectedVehiclesInList--;
					
				VehicleStatusList.erase(findVehicleIDInVehicleStatusList);
				i--;
			}
		}
	}
}
/*
	- The following method can formulate JSON formatted message based on the Vehicle Status List.
	- It matches the request approachId with the vehicle approachId in the Vehicle Status List.
*/
string VehicleStatusManager::getVehicleStatusList(int requested_ApproachId)
{
	string vehicleStatusListJsonString{};
	int noOfVehicle{};
	int temporaryVehicleID{};
	vector<VehicleStatus> TemporaryVehcileStatusList{};
	TemporaryVehcileStatusList = VehicleStatusList;
	Json::Value jsonObject;
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "";

	jsonObject["MsgType"] = "VehicleStatusList";

	for (unsigned int i = 0; i < TemporaryVehcileStatusList.size(); i++)
	{
		temporaryVehicleID = TemporaryVehcileStatusList[i].vehicleId;
		if (requested_ApproachId != TemporaryVehcileStatusList[i].vehicleApproachId)
		{
			vector<VehicleStatus>::iterator findVehicleIDOnList = std::find_if(std::begin(TemporaryVehcileStatusList), std::end(TemporaryVehcileStatusList),
																			   [&](VehicleStatus const &p)
																			   { return p.vehicleId == temporaryVehicleID; });

			TemporaryVehcileStatusList.erase(findVehicleIDOnList);
			i--;
		}
	}

	if (!TemporaryVehcileStatusList.empty())
	{
		for (unsigned int i = 0; i < TemporaryVehcileStatusList.size(); i++)
		{
			noOfVehicle++;
			jsonObject["VehicleStatusList"][i]["vehicleId"] = TemporaryVehcileStatusList[i].vehicleId;
			jsonObject["VehicleStatusList"][i]["vehicleType"] = TemporaryVehcileStatusList[i].vehicleType;
			jsonObject["VehicleStatusList"][i]["speed_MeterPerSecond"] = TemporaryVehcileStatusList[i].vehicleSpeed_MeterPerSecond;
			jsonObject["VehicleStatusList"][i]["heading_Degree"] = TemporaryVehcileStatusList[i].vehicleHeading_Degree;
			jsonObject["VehicleStatusList"][i]["inBoundLaneId"] = TemporaryVehcileStatusList[i].vehicleLaneId;
			jsonObject["VehicleStatusList"][i]["inBoundApproachId"] = TemporaryVehcileStatusList[i].vehicleApproachId;
			jsonObject["VehicleStatusList"][i]["signalGroup"] = TemporaryVehcileStatusList[i].vehicleSignalGroup;
			jsonObject["VehicleStatusList"][i]["distanceFromStopBar"] = TemporaryVehcileStatusList[i].vehicleDistanceFromStopBar;
			// jsonObject["VehicleStatusList"][i]["stoppedDelay"] = TemporaryVehcileStatusList[i].vehicleStoppedDelay;
			jsonObject["VehicleStatusList"][i]["locationOnMap"] = TemporaryVehcileStatusList[i].vehicleLocationOnMap;
			jsonObject["VehicleStatusList"][i]["connectedVehicleStatus"] = TemporaryVehcileStatusList[i].connected;

		}
	}

	jsonObject["NoOfVehicle"] = noOfVehicle;
	vehicleStatusListJsonString = Json::writeString(builder, jsonObject);

	return vehicleStatusListJsonString;
}

string VehicleStatusManager::getVehicleStatusList()
{
	string vehicleStatusListJsonString{};
	int noOfVehicle{};

	Json::Value jsonObject;
	Json::StreamWriterBuilder builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "";

	jsonObject["MsgType"] = "VehicleStatusList";

	if (!VehicleStatusList.empty())
	{
		for (unsigned int i = 0; i < VehicleStatusList.size(); i++)
		{
			noOfVehicle++;
			jsonObject["VehicleStatusList"][i]["vehicleId"] = VehicleStatusList[i].vehicleId;
			jsonObject["VehicleStatusList"][i]["vehicleType"] = VehicleStatusList[i].vehicleType;
			jsonObject["VehicleStatusList"][i]["speed_MeterPerSecond"] = VehicleStatusList[i].vehicleSpeed_MeterPerSecond;
			jsonObject["VehicleStatusList"][i]["heading_Degree"] = VehicleStatusList[i].vehicleHeading_Degree;
			jsonObject["VehicleStatusList"][i]["inBoundLaneId"] = VehicleStatusList[i].vehicleLaneId;
			jsonObject["VehicleStatusList"][i]["inBoundApproachId"] = VehicleStatusList[i].vehicleApproachId;
			jsonObject["VehicleStatusList"][i]["signalGroup"] = VehicleStatusList[i].vehicleSignalGroup;
			jsonObject["VehicleStatusList"][i]["distanceFromStopBar"] = VehicleStatusList[i].vehicleDistanceFromStopBar;
			jsonObject["VehicleStatusList"][i]["locationOnMap"] = VehicleStatusList[i].vehicleLocationOnMap;
			jsonObject["VehicleStatusList"][i]["connectedVehicleStatus"] = VehicleStatusList[i].connected;

		}
	}

	jsonObject["NoOfVehicle"] = noOfVehicle;
	vehicleStatusListJsonString = Json::writeString(builder, jsonObject);
	msgSendingTime = getPosixTimestamp();

	return vehicleStatusListJsonString;
}

bool VehicleStatusManager::checkMsgSendingRequirement()
{
	bool messageSendrequirement{false};
	double currentTime = getPosixTimestamp();

	if (((currentTime - msgSendingTime) >= msgSendingFrequency) && VehicleStatusList.size() >= 3)
		messageSendrequirement = true;

	return messageSendrequirement;
}

VehicleStatusManager::~VehicleStatusManager()
{
	delete plocAwareLib;
}