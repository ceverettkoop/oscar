#pragma once
#include <BWAPI.h>

class GameState;

class Decider{

public:  
    Decider(){};
    
    GameState* gs;
    void onStart();
    void onFrame();
    void onUnitDestroy(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);

private:
    void setScouting();
    void calculateWorkers();
    void updateOwnedBases();
    void assignCombatGroup(BWAPI::Unit unit);
    void setRoles();

};