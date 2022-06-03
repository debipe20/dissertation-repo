struct ArrivalTable
{
    int signalGroup{};
    double distanceToStopbar{};
    double ETA{};
    int vehicleType{};
    double priorityWeight{};
    
    void reset()
    {
        signalGroup = 0;
        distanceToStopbar = 0.0;
        ETA = 0.0;
        vehicleType = 0;
        priorityWeight = 0.0;
    }
};
