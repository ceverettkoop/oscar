//brute commands to units or groups of units
//micro processed elsewhere

#include "GameState.h"
#include "OscarMap.h"
#include "Macro.h"
#include "Decider.h"
#include "Tools.h"

void MacroManager::onFrame(GameState* gs){

    if(gs->scouting) scouting(scout, gs->mapPtr->enemyMain);
}

void MacroManager::scouting(BWAPI::Unit scout, BWEM::Base* enemyMain){

    //will get changed to base when base found
    BWAPI::TilePosition enemyLocation = BWAPI::TilePositions::Unknown;
    
    //assign scout if needed
    if(scout == nullptr || !scout->exists()){ //if scout not assigned or dead
            scout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
    }else scout = nullptr; //if not scouting release any unit from being called a scout

    auto& startLocations = BWAPI::Broodwar->getStartLocations();

    for(BWAPI::TilePosition tpos : startLocations){
        if(BWAPI::Broodwar->isExplored(tpos)) continue; //if we have seen the base skip

        BWAPI::Position pos(tpos);
        scout->move(pos); //otherwise move to it

        if(BWAPI::Broodwar->isExplored(tpos)){ //if we are seeing it for the first time check if enemy is there
            BWAPI::Unitset baseUnits = BWAPI::Broodwar->getUnitsOnTile(tpos);
            for(auto& unit : baseUnits){
                if(BWAPI::Broodwar->self()->isEnemy(unit->getPlayer())) enemyLocation = tpos;
                
                //TODO CAST TPOS INTO A BWEM BASE AND STORE IT IN GS->ENEMYMAIN

            }    
        }
        
        break;
    }


}