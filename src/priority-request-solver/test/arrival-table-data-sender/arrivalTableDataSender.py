import socket
import json
import time

fileName = "ArrivalTable.json"
microseconds = 2.0

# Read a config file into a json object:
configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
config = (json.load(configFile))
configFile.close()

hostIp = config["HostIp"]
port = config["PortNumber"]["SignalCoordination"]
arrivalTableDataSenderSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
arrivalTableDataSenderSocket.bind((hostIp, port))

prirorityRequestSolverPort = config["PortNumber"]["PrioritySolver"]
communicationInfo = (hostIp, prirorityRequestSolverPort)


def getJsonString(fileName):
    f = open(fileName, 'r')
    data = f.read()
    f.close()

    return data

while(True):
    data = getJsonString(fileName)
    arrivalTableDataSenderSocket.sendto(data.encode(), communicationInfo)
    print("Send arrival table data at time", time.time())

    time.sleep(microseconds)

arrivalTableDataSenderSocket.close()
