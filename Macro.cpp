//brute commands to units or groups of units
//micro processed elsewhere

#include "GameState.h"
#include "OscarMap.h"
#include "Macro.h"
#include "Decider.h"
#include "Tools.h"

void MacroManager::onStart(){

}

void MacroManager::onFrame(){

    assignWorkers();
    if(gs->isScouting) scouting();


}

void MacroManager::scouting(){

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

void MacroManager::assignWorkers(){

    //every frame tell every worker to work per gamestate assignments
    //gamestate will update on worker, nexus or refinery creation or destruction
    //logic for worker to flee enemy or fight etc handled in micro which procs after macro and should overwrite this command
    //scouting etc also procs after

    BWAPI::Unitset allWorkers = Tools::GetUnitSetofType(BWAPI::Broodwar->self()->getRace().getWorker());
    BWAPI::Unit worker;
    auto itr = allWorkers.begin();

    //go through bases and assign workers based on what base needs
    for(auto &p : gs->workerTotals){
        
        //otherwise chose a worker
        worker = *itr;
        //skip workers that are not idle and are not mining already; assume if they are doing something else it's important
        while(!worker->isIdle() && !worker->isGatheringMinerals() && !worker->isGatheringGas()  ){
            if (itr != allWorkers.end()){
                 itr++;
                 worker = *itr;
            }else{
                return; //bail if at end of worker set
            }
        }
        
        //now we have a worker idle or already mining... hopefully
        if(p.first.isOccupied){
            //for each mineral patch assign a worker
            for(auto min : p.second->Minerals()){
                
            }
        }

    }



}