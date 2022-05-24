/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    vehicle-prediction-data-collector-main.cpp
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. 

*/
#include "TrafficStateEstimationDataCollector.h"
#include <UdpSocket.h>
#include "json/json.h"

int main()
{
    std::string configFilename("/nojournal/bin/mmitss-phase3-master-config.json");
    Json::Value jsonObject;
    Json::Value receivedJsonObject;
    ifstream configJson(configFilename);
    string configJsonString((std::istreambuf_iterator<char>(configJson)), std::istreambuf_iterator<char>());
    Json::CharReaderBuilder builder;
    Json::CharReader *reader = builder.newCharReader();
    string errors{};
    reader->parse(configJsonString.c_str(), configJsonString.c_str() + configJsonString.size(), &jsonObject, &errors);
    delete reader;

    TrafficStateEstimationDataCollector trafficStateEstimationDataCollector;
    BasicVehicle basicVehicle;

    //Socket Communication
    UdpSocket trafficSateEstimationDataCollectorSocket(static_cast<short unsigned int>(jsonObject["PortNumber"]["DataCollector"].asInt()));
    // const int vehicleStatusManagerPortNo = jsonObject["PortNumber"]["MessageDistributor"].asInt();
    const string HostIp = jsonObject["HostIp"].asString();
    char receiveBuffer[163840];
    int msgType{};
    string vehicleStatusRequestJsonString{};

    while (true)
    {
        trafficSateEstimationDataCollectorSocket.receiveData(receiveBuffer, sizeof(receiveBuffer));
        string receivedJsonString(receiveBuffer);
        msgType = trafficStateEstimationDataCollector.getMessageType(receivedJsonString);

        if (msgType == static_cast<int>(msgType::CurrentState_VehiclePhases))
            //  trafficStateEstimationDataCollector.processSpatData(receivedJsonString);
            trafficStateEstimationDataCollector.updatePhaseStatusInDataPointList(receivedJsonString);

        else if (msgType == static_cast<int>(msgType::VehicleStatus))
            trafficStateEstimationDataCollector.processVehicleStatusData(receivedJsonString);
            // trafficStateEstimationDataCollector.fillUpDataPointList(receivedJsonString);
    }
    
    trafficSateEstimationDataCollectorSocket.closeSocket();
    return 0;
}