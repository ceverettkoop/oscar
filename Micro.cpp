#include "Micro.h"


void moveMutas(BWAPI::Position position, BWAPI::Unitset mutalisks){

    // Find a larva or idle overlord to add to the unit set
    BWAPI::Unit larva = nullptr;
    BWAPI::Unit overlord = nullptr;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType() == BWAPI::UnitTypes::Zerg_Larva && !unit->isMorphing()){
            larva = unit;
            break;
        }
        else if (unit->getType() == BWAPI::UnitTypes::Zerg_Overlord && unit->isIdle()){
            overlord = unit;
            break;
        }
    }
    if (larva){
        mutalisks.insert(larva);
    }
    else if (overlord){
        mutalisks.insert(overlord);
    }
    
    // Issue move command to mutalisks
    for (auto& mutalisk : mutalisks){
        mutalisk->move(position);
    }
    
    // Remove larva/overlord from unit set and cancel its last command if relevant
    if (larva){
        mutalisks.erase(larva);
    }
    else if (overlord)
    {
        mutalisks.erase(overlord);
        overlord->stop();
    }
}
