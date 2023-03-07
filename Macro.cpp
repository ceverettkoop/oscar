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

    BWAPI::UnitType type = BWAPI::Broodwar->self()->getRace().getWorker();

    BWAPI::Unitset idleWorkers;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if (unit->getType() == type && unit->isCompleted() && unit->isIdle()){
            idleWorkers.insert(unit);
        }
    }

    //in theory a set of workers we can steal from because they are assigned to mining but not moving there
    BWAPI::Unitset gatherers;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if (unit->getType() == type && unit->isCompleted() && BWAPI::Orders::WaitForMinerals){
            gatherers.insert(unit);
        }
    }


    BWAPI::Unit worker;

    //FOR EACH BASE
    for(auto &p : gs->workerTotals){
        if(p.first.isOccupied){

            //count workers in base that are gathering minerals and gas
            int minerCount = countMinersInBase(p.second);
            int collectorCount = countGasCollectorsInBase(p.second);

            //GAS 
            //DBL GEYSER WILL BE BROKEN TODO FIX    
            auto geyser = p.second->Geysers().front();

            //while we have less collectors than needed
            while(collectorCount < p.first.onGas){
                
                //try to assign an idle worker first then a waiting min gatherer
                if(idleWorkers.size() > 0){
                    worker =  *idleWorkers.begin();
                    Tools::SmartRightClick(worker, geyser->Unit());
                    collectorCount++;
                    idleWorkers.erase(idleWorkers.begin());
                }else if(gatherers.size() > 0){
                        worker =  *gatherers.begin();
                        Tools::SmartRightClick(worker, geyser->Unit());
                        collectorCount++;
                        gatherers.erase(gatherers.begin());
                    }else break; //if both sets empty give up
            }

            //MINERALS
            //while we have less collectors than needed
            while(minerCount < p.first.onMin){
                //try to assign an idle worker then a gatherer
                if(idleWorkers.size() > 0){
                    worker =  *idleWorkers.begin();

                    //find best mineral, this will be a function later
                    BWAPI::Unit targetMin;
                    for(auto min : p.second->Minerals()){
                        targetMin = min->Unit();
                        if(!min->Unit()->isBeingGathered()) break;
                    }

                    Tools::SmartRightClick(worker, targetMin);
                    minerCount++;
                    idleWorkers.erase(idleWorkers.begin());
                }else if(gatherers.size() > 0){
                        worker =  *gatherers.begin();

                        //find best mineral, this will be a function later
                        BWAPI::Unit targetMin;
                        for(auto min : p.second->Minerals()){
                            targetMin = min->Unit();
                            if(!min->Unit()->isBeingGathered()) break;
                        }

                        Tools::SmartRightClick(worker, targetMin);
                        minerCount++;
                        gatherers.erase(gatherers.begin());
                    }else break; //if both sets empty give up
            }

        }
    }
}


//for speed just counting a miner that is within 20 build tiles of center of base (not ideal)
int MacroManager::countMinersInBase(const BWEM::Base *base){

    int count = 0;

    BWAPI::UnitType type = BWAPI::Broodwar->self()->getRace().getWorker();
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if(unit->getType() = type && unit->isGatheringMinerals()){
            if( unit->getDistance(base->Center()) < 640 ){
                count++;
            }
        }
    }

    return count;

}

//for speed just counting a miner that is within 20 build tiles of center of base (not ideal)
int MacroManager::countGasCollectorsInBase(const BWEM::Base *base){

    int count = 0;

    BWAPI::UnitType type = BWAPI::Broodwar->self()->getRace().getWorker();
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if(unit->getType() = type && unit->isGatheringGas()){
            if( unit->getDistance(base->Center()) < 640 ){
                count++;
            }
        }
    }

    return count;

}