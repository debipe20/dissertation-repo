/*
**********************************************************************************

 © 2019 Arizona Board of Regents on behalf of the University of Arizona with rights
       granted for USDOT OSADP distribution with the Apache 2.0 open source license.

**********************************************************************************

   vehicle-status-manager-main.cpp
    Created by: Debashis Das
    University of Arizona   
    College of Engineering

    This code was developed under the supervision of Professor Larry Head
    in the Systems and Industrial Engineering Department.

    Description:
    
    1. 

*/
#include "VehicleStatusManager.h"
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

    VehicleStatusManager vehicleStatusManager(configFilename);
    BasicVehicle basicVehicle;

    //Socket Communication
    UdpSocket vehicleStatusManagerSocket(static_cast<short unsigned int>(jsonObject["PortNumber"]["TrajectoryAware"].asInt()), 1, 0);
    const string HostIP = jsonObject["HostIp"].asString();
    const int dataCollectorPortNo = jsonObject["PortNumber"]["DataCollector"].asInt();

    char receiveBuffer[10240];
    bool timedOutOccur{};
    string vehicleStatusListJsonString{};

    while (true)
    {
        timedOutOccur = vehicleStatusManagerSocket.receiveData(receiveBuffer, sizeof(receiveBuffer));
        if (timedOutOccur == false)
        {
            string receivedJsonString(receiveBuffer);
            reader->parse(receivedJsonString.c_str(), receivedJsonString.c_str() + receivedJsonString.size(), &receivedJsonObject, &errors);
            

            if (receivedJsonObject["MsgType"] == "BSM")
            {
                // cout << "Received BSM " << receivedJsonString << endl;
                basicVehicle.json2BasicVehicle(receivedJsonString);
                vehicleStatusManager.manageVehicleStatusList(basicVehicle);
            }

            if (vehicleStatusManager.checkMsgSendingRequirement())
            {
                vehicleStatusListJsonString = vehicleStatusManager.getVehicleStatusList();
                cout << "\nSent Vehicle Status \n" << vehicleStatusListJsonString << endl;
                vehicleStatusManagerSocket.sendData(HostIP, static_cast<short unsigned int>(dataCollectorPortNo), vehicleStatusListJsonString);
            }

            //     else if (receivedJsonObject["MsgType"] == "VehicleStatusListRequest")
            //     {
            //         vehicleStatusListJsonString = vehicleStatusManager.getVehicleStatusList(receivedJsonObject["ApproachId"].asInt());
            //         vehicleStatusManagerSocket.sendData(HostIP, static_cast<short unsigned int>(dataCollectorPortNo), vehicleStatusListJsonString);
            //     }
            // }

            else
                vehicleStatusManager.deleteTimedOutVehicleIdFromVehicleStatusList();
        }
    }
    delete reader;
    vehicleStatusManagerSocket.closeSocket();
    return 0;
}
