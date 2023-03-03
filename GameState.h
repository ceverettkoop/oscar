#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"
#include <map>

class OscarMap;

typedef struct baseEconomy{
    int onGas = 0;
    int idealOnGas = 0;
    int onMin = 0;
    int idealOnMin = 0;
    int basePriority = -1;
    int activeBaseCount = 1;
    bool isOccupied = false;
};


class GameState{
    
public:
    bool isScouting = false;
    int workerCount = 0;
    OscarMap* mapPtr = nullptr;
    std::map<baseEconomy, const BWEM::Base*> workerTotals;
    
};