#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace COP
{
    struct PerformanceMeasurement
    {
        int stageNo{};
        int timeStep{};
        int signalGroup1InRing1{};
        int signalGroup2InRing1{};
        int signalGroup1InRing2{};
        int signalGroup2InRing2{};
        int allocatedTimeStepSignalGroup1Ring1{};
        int allocatedTimeStepSignalGroup2Ring1{};
        int allocatedTimeStepSignalGroup1Ring2{};
        int allocatedTimeStepSignalGroup2Ring2{};
        int allocatedGreenTimeSignalGroup1Ring1{};
        int allocatedGreenTimeSignalGroup2Ring1{};
        int allocatedGreenTimeSignalGroup1Ring2{};
        int allocatedGreenTimeSignalGroup2Ring2{};
        int clearanceIntervalSignalGroup1Ring1{};
        int clearanceIntervalSignalGroup2Ring1{};
        int clearanceIntervalSignalGroup1Ring2{};
        int clearanceIntervalSignalGroup2Ring2{};
        int allocatedTimeToPreviousStage{};
        double performmanceMeasurementSignalGroup1Ring1{};
        double performmanceMeasurementSignalGroup2Ring1{};
        double performmanceMeasurementSignalGroup1Ring2{};
        double performmanceMeasurementSignalGroup2Ring2{};
        double activePhasesPerformancemeasurement{};
        double inactivePhasesPerformancemeasurement{};
        double previousStagePerformanceMeasurement{};
        double totalPerformmanceMeasurement{};

        void reset()
        {
            stageNo = 0.0;
            timeStep = 0.0;
            signalGroup1InRing1 = 0;
            signalGroup2InRing1 = 0;
            signalGroup1InRing2 = 0;
            signalGroup2InRing2 = 0;
            allocatedTimeStepSignalGroup1Ring1 = 0;
            allocatedTimeStepSignalGroup2Ring1 = 0;
            allocatedTimeStepSignalGroup1Ring2 = 0;
            allocatedTimeStepSignalGroup2Ring2 = 0;
            allocatedGreenTimeSignalGroup1Ring1 = 0;
            allocatedGreenTimeSignalGroup2Ring1 = 0;
            allocatedGreenTimeSignalGroup1Ring2 = 0;
            allocatedGreenTimeSignalGroup2Ring2 = 0;
            clearanceIntervalSignalGroup1Ring1 = 0;
            clearanceIntervalSignalGroup2Ring1 = 0;
            clearanceIntervalSignalGroup1Ring2 = 0;
            clearanceIntervalSignalGroup2Ring2 = 0;
            allocatedTimeToPreviousStage = 0.0;
            performmanceMeasurementSignalGroup1Ring1 = 0.0;
            performmanceMeasurementSignalGroup2Ring1 = 0.0;
            performmanceMeasurementSignalGroup1Ring2 = 0.0;
            performmanceMeasurementSignalGroup2Ring2 = 0.0;
            activePhasesPerformancemeasurement = 0.0;
            inactivePhasesPerformancemeasurement = 0.0;
            previousStagePerformanceMeasurement = 0.0;
            totalPerformmanceMeasurement = 0.0;
        }
    };
    struct OptimalPlan
    {
        int stageNo{};
        int timeStep{};
        int signalGroup1InRing1{};
        int signalGroup2InRing1{};
        int signalGroup1InRing2{};
        int signalGroup2InRing2{};
        int allocatedGreenTimeSignalGroup1Ring1{};
        int allocatedGreenTimeSignalGroup2Ring1{};
        int allocatedGreenTimeSignalGroup1Ring2{};
        int allocatedGreenTimeSignalGroup2Ring2{};
        int clearanceIntervalSignalGroup1Ring1{};
        int clearanceIntervalSignalGroup2Ring1{};
        int clearanceIntervalSignalGroup1Ring2{};
        int clearanceIntervalSignalGroup2Ring2{};
        double optimalPerformmanceMeasurement{};

        void reset()
        {
            stageNo = 0.0;
            timeStep = 0.0;
            signalGroup1InRing1 = 0;
            signalGroup2InRing1 = 0;
            signalGroup1InRing2 = 0;
            signalGroup2InRing2 = 0;
            allocatedGreenTimeSignalGroup1Ring1 = 0;
            allocatedGreenTimeSignalGroup2Ring1 = 0;
            allocatedGreenTimeSignalGroup1Ring2 = 0;
            allocatedGreenTimeSignalGroup2Ring2 = 0;
            clearanceIntervalSignalGroup1Ring1 = 0;
            clearanceIntervalSignalGroup2Ring1 = 0;
            clearanceIntervalSignalGroup1Ring2 = 0;
            clearanceIntervalSignalGroup2Ring2 = 0;
            optimalPerformmanceMeasurement = 0.0;
        }
    };
    struct plannedSignaBarrierGroup
    {
        vector<int> signaBarrierGroup1{};
        vector<int> signaBarrierGroup2{};

        void reset()
        {
            signaBarrierGroup1.clear();
            signaBarrierGroup2.clear();
        }
    };

    struct StagePlan
    {
        int stageNo{};
        int signalBarrierGroupNo{};
        int noOfPhase{};
        int signalGroup1InRing1{};
        int signalGroup2InRing1{};
        int signalGroup1InRing2{};
        int signalGroup2InRing2{};

        void reset()
        {
            stageNo = 0;
            signalBarrierGroupNo = 0;
            noOfPhase = 0;
            signalGroup1InRing1 = 0;
            signalGroup2InRing1 = 0;
            signalGroup1InRing2 = 0;
            signalGroup2InRing2 = 0;
        }
    };
    struct AvailableOptionForForwardStage
    {
        int signalGroup1{};
        int signalGroup2{};
        int allocatedTimeStepSignalGroup1{};
        int allocatedTimeStepSignalGroup2{};
        int allocatedGreenTimeSignalGroup1{};
        int allocatedGreenTimeSignalGroup2{};
        int minimumGreenTimeSignalGroup1{};
        int minimumGreenTimeSignalGroup2{};
        int clearanceIntervalSignalGroup1{};
        int clearanceIntervalSignalGroup2{};


        void reset()
        {
            signalGroup1 = 0;
            signalGroup2 = 0;
            allocatedTimeStepSignalGroup1 = 0;
            allocatedTimeStepSignalGroup2 = 0;
            allocatedGreenTimeSignalGroup1 = 0;
            allocatedGreenTimeSignalGroup2 = 0;
            minimumGreenTimeSignalGroup1 = 0;
            minimumGreenTimeSignalGroup2 = 0;
            clearanceIntervalSignalGroup1 = 0;
            clearanceIntervalSignalGroup2 = 0;
        }
    };
};
