#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"
#include <vector>

class OscarMap;

struct baseEconomy{

    int onGas = 0;
    int onMin = 0;
    int minCount = 0;
    int assimilatorCount = 0;
    int basePriority = 2; //program should set to 0 if main and 1 if natural
    bool isOccupied = false;
};


class GameState{
    
public:
    int basesDesired = 1;
    bool isScouting = false;
    int workerCount = 0;
    int workerMax = 0; //based on current amount of bases occupied, will change when we expand
    int activeBaseCount = 0;
    OscarMap* mapPtr = nullptr;
    std::vector<std::pair<baseEconomy, const BWEM::Base *>> workerTotals;
    
};