#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"
#include <map>

class OscarMap;

typedef struct baseEcon{
    int onGas;
    int onMin;
    const BWEM::Base* basePtr;

};


class GameState{
    
public:
    bool isScouting = false;
    OscarMap* mapPtr;
    std::map<baseEcon, int> workerTotals;
    
};