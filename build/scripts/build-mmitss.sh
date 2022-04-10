#!/bin/bash
#############################################################################################
#                                                                                           
# NOTICE:  Copyright 2018 Arizona Board of Regents on behalf of University of Arizona.      
# All information, intellectual, and technical concepts contained herein is and shall       
# remain the proprietary information of Arizona Board of Regents and may be covered         
# by U.S. and Foreign Patents, and patents in process.  Dissemination of this information          
# or reproduction of this material is strictly forbidden unless prior written permission    
# is obtained from Arizona Board of Regents or University of Arizona.                       
#                                                                                           
# build-mmitss.sh                                                                     
# Created by Niraj Altekar                                                                  
# Transportation Research Institute                                                         
# Systems and Industrial Engineering                                                        
# The University of Arizona                                                                 
#                                                                                           
# This code was develop under the supervision of Professor Larry Head                       
# in the Transportation Research Institute.                                                 
#                                                                                           
# Operational Description:                                                                   
# This script builds all mmitss applications (vehicle, intersection, and common), 
# and different types of docker images.
#############################################################################################

# Define colors:
red='\033[0;31m'
green='\033[0;32m'
nocolor='\033[0m'

######################################################################################

read -p "Has setup-build-environment.sh script already been executed? (y or n): " buildSetup

if [ "$buildSetup" = "n" ]; then
echo "Please run setup-build-environment.sh script first and then run this script. Exiting now!"
exit 0

else

    read -p "Build all applications? (y or n): " all
    if [ "$all" = "n" ]; then

    read -p "Build common applications? (y or n): " common
    read -p "Build roadside applications? (y or n): " mrp
    read -p "Build simulation_server-tools applications? (y or n): " server

    else
    common=y
    mrp=y
    vsp=y
    server=y
    fi

    ######################################################################################

    ################################## COMMON APPLICATIONS ###############################
    if [ "$common" = "y" ]; then

	    echo "------------------------"
	    echo "COMMON APPLICATIONS"
	    echo "------------------------"

	    #######################################################################################
	    echo "Building System Interface..."
	    cd ../../src/system-interface
	    # Clean the folder and build for linux.
	    pyinstaller --add-data "templates:templates" --add-data "static:static" --additional-hooks-dir=. --onefile --windowed system-interface.py &> /dev/null
	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv dist/system-interface  ../../build/bin/SystemInterface/$PROCESSOR/M_SystemInterface
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the files to keep the folders clean
	    rm -r build dist *.spec &> /dev/null
	    rm -r __pycache__ &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s
	    #######################################################################################
    fi

    ############################### INTERSECTION APPLICATIONS #############################
    if [ "$mrp" = "y" ]; then

	    echo "---------------------"
	    echo "ROADSIDE APPLICATIONS"
	    echo "---------------------"

	    #######################################################################################
	    echo "Building Snmp Engine..."
	    cd ../../src/snmp-engine
	    # Clean the folder and build for linux.
	    make clean &> /dev/null

	    if [ "$PROCESSOR" = "arm" ]; then
		    make linux ARM=1 &> /dev/null
	    else
		    make linux &> /dev/null
	    fi

	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv M_SnmpEngine ../../build/bin/SnmpEngine/$PROCESSOR/M_SnmpEngine
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the .o files to keep the folders clean
	    rm ./*.o &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s
	    ######################################################################################

	    #######################################################################################
	    echo "Building Map Spat Broadcaster..."
	    cd ../../src/map-spat-broadcaster
	    # Clean the folder and build for linux.
	    pyinstaller --hidden-import=pkg_resources.py2_warn --onefile --windowed map-spat-broadcaster.py  &> /dev/null
	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv dist/map-spat-broadcaster  ../../build/bin/MapSpatBroadcaster/$PROCESSOR/M_MapSpatBroadcaster
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the .o files to keep the folders clean
	    rm -r build dist *.spec &> /dev/null
	    rm -r __pycache__ &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s
	    #######################################################################################
	    
	    #######################################################################################
	    echo "Building VehicleStatus Manager..."
	    cd ../../src/vehicle-status-manager
	    # Clean the folder and build for linux.
	    make clean &> /dev/null

	    if [ "$PROCESSOR" = "arm" ]; then
		    make linux ARM=1 &> /dev/null
	    else
		    make linux &> /dev/null
	    fi

	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv M_VehicleStatusManager ../../build/bin/VehicleStatusManager/$PROCESSOR/M_VehicleStatusManager
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the .o files to keep the folders clean
	    rm ./*.o &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s
	    ######################################################################################
	    
	    
	    #######################################################################################
	    echo "Building Data Collector..."
	    cd ../../src/data-collector
	    # Clean the folder and build for linux.
	    make clean &> /dev/null

	    if [ "$PROCESSOR" = "arm" ]; then
		    make linux ARM=1 &> /dev/null
	    else
		    make linux &> /dev/null
	    fi

	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv M_VehicleStatusPredictionDataCollector ../../build/bin/DataCollector/$PROCESSOR/M_VehicleStatusPredictionDataCollector
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the .o files to keep the folders clean
	    rm ./*.o &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s
	    ######################################################################################
    fi

 
    ################################### SERVER TOOLS ##################################

    if [ "$server" = "y" ]; then

	    echo "------------------------------------"
	    echo "SIMULATION_SERVER-TOOLS APPLICATIONS"
	    echo "------------------------------------"

	    #######################################################################################
	    echo "Building Message Distributor..."
	    cd ../../src/message-distributor
	    # Clean the folder and build for linux.
	    pyinstaller --hidden-import=pkg_resources.py2_warn --onefile --windowed message-distributor.py  &> /dev/null
	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv dist/message-distributor  ../../build/bin/MessageDistributor/$PROCESSOR/M_MessageDistributor
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the .o files to keep the folders clean
	    rm -r build dist *.spec &> /dev/null
	    rm -r __pycache__ &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s
	    #######################################################################################

	    #######################################################################################
	    echo "Building Simulated BSM Blob Processor..."
	    cd ../../src/simulated-bsm-blob-processor
	    # Clean the folder and build for linux.
	    pyinstaller --hidden-import=pkg_resources.py2_warn --onefile --windowed simulated-bsm-blob-processor.py  &> /dev/null
	    # Indicate Success/Failure of the build
	    if [ "$?" -eq "0" ]; then
		    mv dist/simulated-bsm-blob-processor  ../../build/bin/SimulatedBsmBlobProcessor/$PROCESSOR/M_SimulatedBsmBlobProcessor
		    echo -e "${green}Successful${nocolor}"
	    else
		    echo -e "${red}Failed${nocolor}"
	    fi
	    # Remove the .o files to keep the folders clean
	    rm -r build dist *.spec &> /dev/null
	    rm -r __pycache__ &> /dev/null
	    # Return back to original directory to go over the process again for another one
	    cd - &> /dev/null
	    sleep 1s

	    #######################################################################################
    fi

    echo "------------------------------------------"
	echo "Successfully built required applications!"
    echo "------------------------------------------"

    read -p "Build docker images? (y or n): " docker
    if [ "$docker" = "y" ]; then
		
		read -p "Build Base image? (y or n): " baseImage
		if [ "$baseImage" = "y" ]; then
			read -p "Provide version tag for the base image: " baseVersionTag
			# Go to the mmitss directory
			cd ../..
			echo "----------------------------------"
			echo "Building Base image for $PROCESSOR"
			echo "---------------------------------------"
				docker build -t mmitssuarizona/mmitss-$PROCESSOR-base:$baseVersionTag -f build/dockerfiles/$PROCESSOR/Dockerfile.base .
			echo "Successfully built the base image with tag: $baseVersionTag" 
			echo "Please note that this new base image will not be used for building other container images unless Dockerfiles for building container images are updated!"
	    else
			# Anyhow, go the mmitss directory
			cd ../..
		fi	

		read -p "Build container images? (y or n): " containerImages
	    
		if [ "$containerImages" = "y" ]; then
			read -p "Provide version tag for container images: " versionTag
			read -p "Build MRP image? Needs common and roadside applications. (y or n): " mrpFieldImage
			read -p "Build simulation_server-tools image? Needs simulation_server-tools applications. (y or n): " serverImage

			if [ "$mrpFieldImage" = "y" ]; then
				echo "---------------------------------------"
				echo "Building MRP-Field image for $PROCESSOR"
				echo "---------------------------------------"
				docker build -t mmitssuarizona/debashis-dissertation-mrp-$PROCESSOR:$versionTag -f build/dockerfiles/$PROCESSOR/Dockerfile.mrp .
			fi	

			if [ "$serverImage" = "y" ]; then
				echo "-----------------------------------------------------"
				echo "Building Simulation_Server-Tools image for $PROCESSOR"
				echo "-----------------------------------------------------"
				docker build -t mmitssuarizona/debashis-dissertation-simulation_server-tools-$PROCESSOR:$versionTag -f build/dockerfiles/$PROCESSOR/Dockerfile.simulation_server-tools .
			fi	
		fi
		echo "------------------------------------------"
		echo "Successfully built required docker images!"
		echo "------------------------------------------"

    fi
fi
