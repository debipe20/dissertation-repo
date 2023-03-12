import json
# import socket

# configFile = open("/nojournal/bin/mmitss-phase3-master-config.json", 'r')
# config = (json.load(configFile))
# configFile.close()

# hostIp = config["HostIp"]
# port = config["PortNumber"]["PrioritySolver"]
# priorityRequestSolverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
# priorityRequestSolverSocket.bind((hostIp, port))

# tciPort = config["PortNumber"]["TrafficControllerInterface"]
# tciCommunicationInfo = (hostIp, tciPort)

# # Receive data on the TCI socket
# data, address = tciSocket.recvfrom(10240)
# data = data.decode()
# # Load the received data into a json object
# receivedMessage = json.loads(data)
# if receivedMessage["MsgType"] == "ActiveTimingPlan":
#     getCurrentSignalTimingPlan
gmin = []
gmax = []
redClearance = []
yellowChange = []

Green = 1
Yellow = 2
Red = 4

def getCurrentSignalTimingPlan():

    currentSignalPlanFile = open("signalPlan.json", 'r')
    signalPlanData = (json.load(currentSignalPlanFile))

    [gmin.append(minGreenTime)
     for minGreenTime in signalPlanData['TimingPlan']['MinGreen']]
    [gmax.append(maxGreenTime)
     for maxGreenTime in signalPlanData['TimingPlan']['MaxGreen']]
    [redClearance.append(redTime)
     for redTime in signalPlanData['TimingPlan']['RedClear']]
    [yellowChange.append(yellowTime)
     for yellowTime in signalPlanData['TimingPlan']['YellowChange']]

    currentSignalPlanFile.close()

    return signalPlanData

def getNextSignalPhaseStatus(desiredPhaseNumber, phaseStatus, elapsedTime, timeStep):
    maxGreen = gmax[desiredPhaseNumber - 1]
    maxYellow = yellowChange[desiredPhaseNumber - 1]
    phaseNumberInRing1 = [1,2,3,4]
    phaseNumberInRing2 = [5,6,7,8]
    
    maxRed = 0
    nextPhaseStatus = Green
    nextPhaseElapsedTime = 0.0
    
    if desiredPhaseNumber in phaseNumberInRing1:
        for phase in phaseNumberInRing1:
            x = gmax[phase - 1] + redClearance[phase -1 ] + yellowChange[phase - 1]
            maxRed = maxRed + x
        maxRed = maxRed - maxGreen -maxYellow 
    else:
        for phase in phaseNumberInRing2:
            x = gmax[phase - 1]
            maxRed = maxRed + x
        maxRed = maxRed - maxGreen
    print("MaxRed is: ", maxRed)     
    
    #Green2Green
    if phaseStatus == Green and maxGreen - elapsedTime >= timeStep:
        nextPhaseStatus = Green
        nextPhaseElapsedTime = elapsedTime + timeStep
    
    #Green2Yellow
    elif phaseStatus == Green and maxGreen - elapsedTime < timeStep:
        nextPhaseStatus = Yellow
        nextPhaseElapsedTime = timeStep + maxGreen - elapsedTime
    
    #Yellow2Yellow
    elif phaseStatus == Yellow and maxYellow - elapsedTime >= timeStep:
        nextPhaseStatus = Yellow
        nextPhaseElapsedTime = elapsedTime + timeStep
    
    #Yellow2Red
    elif phaseStatus == Yellow and maxYellow - elapsedTime < timeStep:
        nextPhaseStatus = Red
        nextPhaseElapsedTime = timeStep + maxYellow - elapsedTime

    #Red2Red
    elif phaseStatus == Red and maxRed - elapsedTime >= timeStep:
        nextPhaseStatus = Red
        nextPhaseElapsedTime = elapsedTime + timeStep
    
    #Red2Green
    elif phaseStatus == Yellow and maxRed - elapsedTime < timeStep:
        nextPhaseStatus = Green
        nextPhaseElapsedTime = timeStep + maxRed - elapsedTime
    
    if nextPhaseElapsedTime < 0:
        nextPhaseElapsedTime = 0

        
    return nextPhaseStatus, nextPhaseElapsedTime

getCurrentSignalTimingPlan()
print(gmin)
print(gmax)
print(redClearance)
print(yellowChange)

print(getNextSignalPhaseStatus(2, 4, 10.37, 0.3))

