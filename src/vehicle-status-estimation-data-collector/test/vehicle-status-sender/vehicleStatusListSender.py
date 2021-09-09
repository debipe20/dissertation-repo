import socket
import json
import datetime
import time

fileName = "vehicle-status-list.json"
microseconds = 0.1

# Read a config file into a json object:
configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
config = (json.load(configFile))
configFile.close()

hostIp = config["HostIp"]
port = config["PortNumber"]["PeerToPeerPriority"]
vehicleStatusListSenderSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
vehicleStatusListSenderSocket.bind((hostIp,port))

dataCollectorPort = config["PortNumber"]["DataCollector"]
communicationInfo = (hostIp, dataCollectorPort)

vehicleStatusListSendingTime = 0.0

while True:
    if time.time() - vehicleStatusListSendingTime >= 0.1:
        f = open(fileName, 'r')
        data = f.read() 
        vehicleStatusListSenderSocket.sendto(data.encode(),communicationInfo)
        vehicleStatusListSendingTime = time.time()
        print("sent vehicle status list at time", time.time())

f.close()
vehicleStatusListSenderSocket.close()