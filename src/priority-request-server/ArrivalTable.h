

struct ArrivalTable
{
    int vehicleType{};
    int vehicleSignalGroup{};
    double vehicleETA{};
    double etaUpdateTime{};

    void reset()
    {
        vehicleType = 0;
        vehicleSignalGroup = 0;
        vehicleETA = 0.0;
        etaUpdateTime = 0.0;
    }
};
