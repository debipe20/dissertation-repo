import socket
import json
import datetime
import time

fileName = "phaseStatus.json"

microseconds = 0.1

# Read a config file into a json object:
configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
config = (json.load(configFile))
configFile.close()

hostIp = config["HostIp"]
port = config["PortNumber"]["MapSPaTBroadcaster"]
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((hostIp,port))

dataProcessorPort = config["PortNumber"]["DataCollector"]
communicationInfo = (hostIp, dataProcessorPort)

dataSendingTime = 0.0

while True:
    if time.time() - dataSendingTime >= 0.1:
        f = open(fileName, 'r')
        data = f.read() 
        s.sendto(data.encode(),communicationInfo)
        dataSendingTime = time.time()
        print("sent phase status at time", time.time())
        
        time.sleep(microseconds)

f.close()
s.close()