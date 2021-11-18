import json 
from SpatDataManager import SpatDataManager
from BsmDataManager import BsmDataManager
from TimeSpaceDiagramManager import TimeSpaceDiagramManager


def main():   
    # Read the config file into a json object:
    configFile = open("configuration.json", 'r')
    config = (json.load(configFile))
    # Close the config file:
    configFile.close()

    xAxisRange = config["EndTimeOfDiagram"] - config["StartTimeOfDiagram"]


    spatDataManager = SpatDataManager(config)
    bsmDataManager = BsmDataManager(config)
    timeSpaceDiagramManager = TimeSpaceDiagramManager()

    """
    Process SPaT files
    """ 
    greenTimeStartPoint, clearanceTimeStartPoint, greenTime, clearanceTime, intersectionDistance_Green, intersectionDistance_Clearance = spatDataManager.manageSpatData()

    """
    Process BSM files
    """
    
    evTrajectoryTimePoint, evTrajectoryDistancePoint, transitTrajectoryTimePoint, transitTrajectoryDistancePoint, truckTrajectoryTimePoint, truckTrajectoryDistancePoint, carTrajectoryTimePoint, carTrajectoryDistancePoint, connectedVehicleTrajectoryTimePoint, connectedVehicleTrajectoryDistancePoint = bsmDataManager.manageBsmData()

    """
    Plot Time-Space Diagram
    """
    timeSpaceDiagramManager.getParameters(greenTimeStartPoint, clearanceTimeStartPoint,
                     greenTime, clearanceTime, intersectionDistance_Green, intersectionDistance_Clearance, evTrajectoryTimePoint, evTrajectoryDistancePoint, transitTrajectoryTimePoint, transitTrajectoryDistancePoint, truckTrajectoryTimePoint, truckTrajectoryDistancePoint, carTrajectoryTimePoint, carTrajectoryDistancePoint, connectedVehicleTrajectoryTimePoint, connectedVehicleTrajectoryDistancePoint, "Time-Space Diagram", xAxisRange)

    timeSpaceDiagramManager.timeSpaceDiagramMethod()
    
if __name__ == "__main__":
    main()