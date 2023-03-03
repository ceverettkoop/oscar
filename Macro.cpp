//brute commands to units or groups of units
//micro processed elsewhere

#include "GameState.h"
#include "OscarMap.h"
#include "Macro.h"
#include "Decider.h"
#include "Tools.h"

void MacroManager::onFrame(GameState* gs){

    if(gs->isScouting) scouting(gs);
}

void MacroManager::scouting(GameState* gs){

    //will get changed to base when base found
    BWAPI::TilePosition enemyLocation = BWAPI::TilePositions::Unknown;
    
    //assign scout if needed
    if(scout == nullptr || !scout->exists()){ //if scout not assigned or dead
            scout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
    }

    auto& startLocations = BWAPI::Broodwar->getStartLocations();

    for(BWAPI::TilePosition tpos : startLocations){

        if(BWAPI::Broodwar->isExplored(tpos)){ //if we see an enemy unit on the start position mark the base
            BWAPI::Unitset baseUnits = BWAPI::Broodwar->getUnitsOnTile(tpos);
            for(auto& unit : baseUnits){
                if(BWAPI::Broodwar->self()->isEnemy(unit->getPlayer())){
                     enemyLocation = tpos;
                }
                
                for(auto& base : gs->mapPtr->mainBases){
                    if(base->Location() == tpos){
                        gs->mapPtr->enemyMain = base;
                        gs->isScouting = false;    //end scouting now
                    }
                }
            }    
            continue; //not directing movement to explored start locations
        }

        BWAPI::Position pos(tpos);
        scout->move(pos); //otherwise move to it
        
        break;
    }


}