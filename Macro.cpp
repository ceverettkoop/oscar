//brute commands to units or groups of units
//micro happens after

#include "Macro.h"
#include "Tools.h"

void MacroManager::onFrame(){

    //are we scouting?
    //TODO MOVE THIS ELSEWHERE
    if(enemyLocation == BWAPI::TilePositions::Unknown){
        if(isScouting){
            if(BWAPI::Broodwar->self()->supplyUsed() > 16){
                isScouting = true;
            }
        }
    }else(isScouting = false); //this is if we found the enemy

    if(isScouting) scouting(scout, enemyLocation);

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