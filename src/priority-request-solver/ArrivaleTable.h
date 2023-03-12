struct ArrivaleTable
{
    int signalGroup{};
    double distanceToStopbar{};
    double ETA{};
    
    void reset()
    {
        signalGroup = 0;
        distanceToStopbar = 0.0;
        ETA = 0.0;
    }
};
