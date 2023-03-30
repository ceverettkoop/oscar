#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"
#include <vector>
#include <map>

class OscarMap;
class BuildQueue;

enum GroupRole{
    ATTACK,
    DEFEND,
    HARASS
};

enum Instruction{
    EXPAND_NATURAL,
    HATCHERY_MAIN,
    LING_SPEED,
    STOP_DRONING,
    START_DRONING,
    NO_INSTRUCTION
};

struct InstructionEntry{
    Instruction inst = NO_INSTRUCTION;
    int supply = 0;
    int time = 0;
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
    bool hatcheryMain = false;
    bool droning = true;
    
    int workerCount = 0;
    int workerMax = 0; //based on current amount of bases occupied, will change when we expand
    int activeBaseCount = 0;
    OscarMap* mapPtr = nullptr;
    BuildQueue* bq = nullptr;
    std::map<int, BaseEconomy> ownedBases;
    std::map<int, CombatGroup> combatGroups;
    std::vector<InstructionEntry> instructions;

};

