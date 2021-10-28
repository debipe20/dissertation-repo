import socket
import json
import datetime
import time

fileName = "bsm.json"
fileName2 = "bsm2.json"
fileName3 = "bsm3.json"
fileName4 = "bsm4.json"
microseconds = 0.1

# Read a config file into a json object:
configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
config = (json.load(configFile))
configFile.close()

hostIp = config["HostIp"]
port = config["PortNumber"]["HostBsmDecoder"]
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind((hostIp,port))

bsmProcessorPort = config["PortNumber"]["MessageDistributor"]
communicationInfo = (hostIp, bsmProcessorPort)

bsmSendingTime = 0.0

while True:
    if time.time()- bsmSendingTime >= 0.3:
        f = open(fileName, 'r')
        data = f.read() 
        s.sendto(data.encode(),communicationInfo)
        bsmSendingTime = time.time()
        print("sent BSM at time", time.time())
        
        time.sleep(microseconds)

        f = open(fileName2, 'r')
        data = f.read() 
        s.sendto(data.encode(),communicationInfo)
        bsmSendingTime = time.time()
        print("sent second BSM at time", time.time())

        time.sleep(microseconds)

        f = open(fileName3, 'r')
        data = f.read() 
        s.sendto(data.encode(),communicationInfo)
        bsmSendingTime = time.time()
        print("sent third BSM at time", time.time())

        time.sleep(microseconds)

        f = open(fileName4, 'r')
        data = f.read() 
        s.sendto(data.encode(),communicationInfo)
        bsmSendingTime = time.time()

        print("sent fourth BSM at time", time.time())
        
        time.sleep(microseconds)

f.close()
s.close()