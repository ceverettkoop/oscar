#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"

class OscarMap;

class GameState{
    
public:
    bool isScouting = false;
    OscarMap* mapPtr;
};