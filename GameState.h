#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"

class OscarMap;

typedef struct baseEcon{
    int onGas;
    int onMin;

};

class GameState{
    
public:
    bool isScouting = false;
    OscarMap* mapPtr;
    
};