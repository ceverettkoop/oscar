#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"
#include <vector>
#include <map>

class OscarMap;

enum GroupRole{
    ATTACK,
    DEFEND,
    HARASS
};

enum IboInstruction{
    NO_INSTRUCTION,
    SCOUT,
    EXPAND
};


struct BaseEconomy{

    const BWEM::Base * base;
    int minCount = 0;
    int assimilatorCount = 0;
    int minerCount = 0;
    int gasMinerCount = 0;
    //int basePriority = 3; //program should set to 0 if main and 1 if natural

};

struct CombatGroup{

    BWAPI::Unitset group;
    GroupRole role;

};


class GameState{
    
public:
    int basesDesired = 1;
    bool isScouting = false;
    int workerCount = 0;
    int workerMax = 0; //based on current amount of bases occupied, will change when we expand
    int activeBaseCount = 0;
    OscarMap* mapPtr = nullptr;
    std::map<int, BaseEconomy> ownedBases;
    std::map<int, CombatGroup> combatGroups;
    IboInstruction instruction = NO_INSTRUCTION;

    void passInstruction(const std::string instring);

};

