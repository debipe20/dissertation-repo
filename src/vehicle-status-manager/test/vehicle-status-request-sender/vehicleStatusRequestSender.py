import socket
import json
import datetime
import time

fileName = "vehicleStatusListRequest.json"

# Read a config file into a json object:
configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
config = (json.load(configFile))
configFile.close()

hostIp = config["HostIp"]
port = config["PortNumber"]["DataCollector"]
dataCollectorSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
dataCollectorSocket.bind((hostIp,port))
dataCollectorSocket.settimeout(0)

bsmProcessorPort = config["PortNumber"]["MessageDistributor"]
communicationInfo = (hostIp, bsmProcessorPort)

msgSendingTime = 0.0

while True:
    try:
        data, address = dataCollectorSocket.recvfrom(10240)
        data = data.decode()
        receivedMessage = json.loads(data)
        if receivedMessage["MsgType"] == "VehicleStatusList":
            print("\n[" + str(datetime.datetime.now()) + "] " + "Received vehicle status list at time " + str(time.time())+ " is following: \n", receivedMessage)
                   
    except:
        if time.time()- msgSendingTime >= 0.1:
            f = open(fileName, 'r')
            data = f.read() 
            dataCollectorSocket.sendto(data.encode(),communicationInfo)
            msgSendingTime = time.time()
            print("\n[" + str(datetime.datetime.now()) + "] " + "sent Vehicle Status request message at time " + str(time.time()))    
        
            f.close()
dataCollectorSocket.close()
