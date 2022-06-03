import socket
import json
import time

microseconds = 2.0
fileName = "arrivalTable.json"

# Read a config file into a json object:
configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
config = (json.load(configFile))
configFile.close()

hostIp = config["HostIp"]
port = config["PortNumber"]["ArrivalDataManager"]
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((hostIp,port))

prirorityRequestServerPort = config["PortNumber"]["PriorityRequestServer"]
communicationInfo = (hostIp, prirorityRequestServerPort)

while True:
    f = open(fileName, 'r')
    data = f.read()
    s.sendto(data.encode(),communicationInfo)
    print (time.time())
    print(data.encode())

    time.sleep(microseconds)
    
f.close()
s.close()