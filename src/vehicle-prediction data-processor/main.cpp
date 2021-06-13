/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    main.cpp
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. 

*/
#include "VehiclePredictionDataProcessor.h"
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

    VehiclePredictionDataProcessor vehiclePredictionDataProcessor(configFilename);
    BasicVehicle basicVehicle;

    //Socket Communication
    UdpSocket vehiclePredictionDataProcessorSocket(static_cast<short unsigned int>(jsonObject["PortNumber"]["MessageDistributor"].asInt()));

    char receiveBuffer[40960];

    while (true)
    {
        vehiclePredictionDataProcessorSocket.receiveData(receiveBuffer, sizeof(receiveBuffer));
        string receivedJsonString(receiveBuffer);

        if (vehiclePredictionDataProcessor.getMessageType(receivedJsonString) == MsgEnum::DSRCmsgID_bsm)
        {
            basicVehicle.json2BasicVehicle(receivedJsonString);
            vehiclePredictionDataProcessor.getVehicleInformationFromMAP(basicVehicle);
        }
    }

    delete reader;
    vehiclePredictionDataProcessorSocket.closeSocket();
    return 0;
}