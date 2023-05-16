#include "Macro.h"


//takes gamestate and returns a set of all units that should be issued a command this frame
BWAPI::Unitset Macro::unitsToAct(const GameState& gs) {

    BWAPI::Unitset returnSet = gs.myUnits;

    //remove buildings and units turning into buildings or constructing
    for (auto& unit : gs.myUnits){
        if(!unit->getType().isBuilding() && !unit->isMorphing()){
            returnSet.erase(unit);
        }

        if
    }
    




    return returnSet;
}