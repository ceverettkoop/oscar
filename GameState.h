#pragma once
#include <BWAPI.h>
#include "BWEM/bwem.h"
#include <vector>
#include <map>

class OscarMap;
class BuildQueue;

enum Owner{
    ALLIED,
    NEUTRAL,
    ENEMY
};

//passed to functions determining behavior that require top level information
//theory is to only pass as a constant argument and to only change one value at a time from
//a function not taking that value
class GameState{
    

private:
//modifiable variables not passed to non gs functions
    BWAPI::Player m_me;
    BWAPI::Player m_enemy;   
    BWAPI::Unitset m_myUnits;
    BWAPI::Unitset m_otherUnits;
    BWAPI::Race m_selfRace = BWAPI::Races::Unknown;
    BWAPI::Race m_enemyRace = BWAPI::Races::Unknown;
    OscarMap m_map = OscarMap();


public:

    GameState(){};

    //basic information set at start
    const BWAPI::Player& me = m_me;
    const BWAPI::Player& enemy = m_enemy;
    const BWAPI::Race& selfRace = m_selfRace;
    const BWAPI::Race& enemyRace = m_enemyRace; 

    //all known units
    const BWAPI::Unitset& myUnits = m_myUnits;
    const BWAPI::Unitset& otherUnits = m_otherUnits;

    //this class includes knowledge of bases, economy, ownership
    const OscarMap& map = m_map;


    //initializing etc
    void onStart();

    //on frame updates
    void MapOnFrame();
    void UpdateUnits();

};

