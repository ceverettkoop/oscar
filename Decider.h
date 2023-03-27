#pragma once
#include <BWAPI.h>
#include "GameState.h"

class Decider{

public:  
    Decider(){};
    
    GameState* gs;
    void onStart();
    void onFrame();
    void onUnitDestroy(BWAPI::Unit unit);
    void onUnitComplete(BWAPI::Unit unit);
    void onUnitMorph(BWAPI::Unit unit);

private:
    void setScouting();
    void calculateWorkers();
    void updateOwnedBases();
    void assignCombatGroup(BWAPI::Unit unit);
    void setRoles();
    void followInstructions();
    bool doInstruction(Instruction inst);

};