//brute commands to units or groups of units
//micro happens after

#include "GameState.h"
#include "Macro.h"
#include "Decider.h"
#include "Tools.h"

void MacroManager::onFrame(GameState* gs){

    if(gs->scouting) scouting(scout, enemyLocation);
}

void MacroManager::scouting(BWAPI::Unit scout, BWAPI::TilePosition enemyLocation){

    //assign scout if needed
    if(scout == nullptr || !scout->exists()){ //if scout not assigned or dead
            scout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
    }else scout = nullptr; //if not scouting release any unit from being called a scout

    auto& startLocations = BWAPI::Broodwar->getStartLocations();

    for(BWAPI::TilePosition tpos : startLocations){
        if(BWAPI::Broodwar->isExplored(tpos)) continue;

        BWAPI::Position pos(tpos);
        scout->move(pos);

        if(BWAPI::Broodwar->isExplored(tpos)){
            BWAPI::Unitset baseUnits = BWAPI::Broodwar->getUnitsOnTile(tpos);
            for(auto& unit : baseUnits){
                if(BWAPI::Broodwar->self()->isEnemy(unit->getPlayer())) enemyLocation = tpos;
            }    
        }
        
        break;
    }


}