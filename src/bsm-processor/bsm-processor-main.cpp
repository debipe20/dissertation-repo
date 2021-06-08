/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

    bsm-processor-main.cpp
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. 

*/
#include "BsmProcessor.h"
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

    BsmProcessor bsmProcessor(configFilename);
    BasicVehicle basicVehicle;

    //Socket Communication
    UdpSocket bsmProcessorSocket(static_cast<short unsigned int>(jsonObject["PortNumber"]["TrajectoryAware"].asInt()));

    char receiveBuffer[40960];
    int msgType{};
    while (true)
    {
        bsmProcessorSocket.receiveData(receiveBuffer, sizeof(receiveBuffer));
        string receivedJsonString(receiveBuffer);
        cout << "Received Message is " << receivedJsonString << endl;
        msgType = bsmProcessor.getMessageType(receivedJsonString);
        if (msgType == MsgEnum::DSRCmsgID_bsm)
        {
            basicVehicle.json2BasicVehicle(receivedJsonString);
            bsmProcessor.getVehicleInformationFromMAP(basicVehicle);
        }
    }

    delete reader;
    bsmProcessorSocket.closeSocket();
    return 0;
}