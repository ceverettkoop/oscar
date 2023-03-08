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
           
        //find any worker except don't steal a builder! this will break our build logic
        for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
            //if worker and complete and was not just told to build
            if (unit->getType() == BWAPI::Broodwar->self()->getRace().getWorker() && unit->isCompleted()
                && unit->getLastCommand().getType() != BWAPI::UnitCommandTypes::Build ){
                scout = unit;
            }
        }
    }

    //first thing is first if we haven't explored our natural do so now to not eff expansion later
    //assign our natural if we haven't already
    if(gs->mapPtr->myNatural == nullptr){
        gs->mapPtr->assignNatural();
    }

    //if we haven't explored natural yet do so; specifically head towards each min until all are revealed!
    if(!natExplored){
        for(auto &min : gs->mapPtr->myNatural->Minerals() ){
            if(BWAPI::Broodwar->isExplored(min->BottomRight())){
                continue;
            }
            scout->move(min->Pos());
            return; //bail if we didn't make it through the list
        }
    }
    //if made it this far nat is explored
    natExplored = true;

    if(!BWAPI::Broodwar->isExplored(gs->mapPtr->myNatural->Location())){
        scout->move(gs->mapPtr->myNatural->Center());
        return; //skip the rest
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

    //global to all bases
    BWAPI::Unitset idleWorkers;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if (unit->getType() == type && unit->isCompleted() && unit->isIdle()){
            idleWorkers.insert(unit);
        }
    }

    BWAPI::Unit worker;

    //FOR EACH BASE
    for(auto &p : gs->workerTotals){
        
        //reset eligible gatherers that could be moved
        BWAPI::Unitset localMiners;
        localMiners.clear();

        BWAPI::Unitset otherMiners;
        otherMiners.clear();

        if(p.first.isOccupied){

            //create set of localMiners for this base
            for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
                BWAPI::Unit target = unit->getOrderTarget();
                if (unit->getType() == type && unit->isCompleted() && unit->isGatheringMinerals()){ 
                    if (target != nullptr){
                        if((unit->getOrderTarget()->getDistance(p.second->Center()) < 640 )){
                            localMiners.insert(unit);
                        }
                    }
                }
            }

            //create set of otherMiners
            for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
                BWAPI::Unit target = unit->getOrderTarget();
                if (unit->getType() == type && unit->isCompleted() && unit->isGatheringMinerals()){ 
                    if (target != nullptr){
                        if((unit->getOrderTarget()->getDistance(p.second->Center()) > 640 )){
                            otherMiners.insert(unit);
                        }
                    }
                }
            }

            //count workers in base that are gathering minerals and gas
            int minerCount = countMinersInBase(p.second);
            int collectorCount = countGasCollectorsInBase(p.second);

            //GAS 
            //DBL GEYSER WILL BE BROKEN TODO FIX    
            auto geyser = p.second->Geysers().front();

            //while we have less gas collectors than needed
            while(collectorCount < p.first.onGas){
                
                //try to assign an idle worker first then steal a local miner
                if(idleWorkers.size() > 0){
                    worker =  *idleWorkers.begin();
                    Tools::SmartRightClick(worker, geyser->Unit());
                    collectorCount++;
                    idleWorkers.erase(idleWorkers.begin());
                }else if(localMiners.size() > 0){
                        worker =  *localMiners.begin();
                        Tools::SmartRightClick(worker, geyser->Unit());
                        collectorCount++;
                        localMiners.erase(localMiners.begin());
                    }else break; //if both sets empty give up
            }

            //MINERALS
            //while we have less min collectors than needed
            while(minerCount < p.first.onMin ){

                //first try to assign an idle worker
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
                
                //if idle does not bring us up to count steal one from elsewhere
                }else if(otherMiners.size() > 0){
                        worker =  *otherMiners.begin();

                        //find best mineral, this will be a function later
                        BWAPI::Unit targetMin;
                        for(auto min : p.second->Minerals()){
                            targetMin = min->Unit();
                            if(!min->Unit()->isBeingGathered()) break;
                        }

                        Tools::SmartRightClick(worker, targetMin);
                        minerCount++;
                        otherMiners.erase(otherMiners.begin());
                    }else break; //if both sets empty give up
            }
        }
    }

    //if after all that we still have idle workers send them to mine at first base listed as occupied
    while(idleWorkers.size() > 0){
        
        const BWEM::Base *base = nullptr;

        worker =  *idleWorkers.begin();
        for(auto &p : gs->workerTotals){
            if (p.first.isOccupied){
                base = p.second;
                break;
            }
        }

        if(base == nullptr) break;

        //find best mineral, this will be a function later
        BWAPI::Unit targetMin;
        for(auto min : base->Minerals()){
            targetMin = min->Unit();
            if(!min->Unit()->isBeingGathered()) break;
        }

        Tools::SmartRightClick(worker, targetMin);
        idleWorkers.erase(idleWorkers.begin());
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