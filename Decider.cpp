#include "Decider.h"
#include "OscarMap.h"
#include "GameState.h"
#include "Tools.h"


void Decider::onStart(){

    //initialize tracker of bases to know which is ours
    for (auto &area : BWEM::Map::Instance().Areas()) {
        
        for (auto &base : area.Bases()) {
                bool isStart = (gs->mapPtr->myMain->Center() == base.Center() );
                baseEconomy newEntry;
                newEntry.isOccupied = isStart;
                gs->workerTotals.insert({newEntry, &base});
        }
    }

}


void Decider::onFrame(){

    //decide if we are scouting
    setScouting();

}

void Decider::onUnitComplete(BWAPI::Unit unit){

    //if it's a nexus also update our count of what bases are ours
    if(unit->getType().isResourceDepot()){ 
        updateOwnedBases();
    }

    //we're only going to recalculate worker assignments if it's a nexus or a worker OR refinery
    if(unit->getType().isWorker() ||  unit->getType().isResourceDepot() || unit->getType().isRefinery()){
        calculateWorkers();
    }

}

void Decider::onUnitDestroy(BWAPI::Unit unit){

    //if it's a nexus also update our count of what bases are ours
    if(unit->getType().isResourceDepot()){ 
        updateOwnedBases();
    }

    //we're only going to recalculate worker assignments if it's a nexus or a worker OR refinery
    if(unit->getType().isWorker() ||  unit->getType().isResourceDepot() || unit->getType().isRefinery()){
        calculateWorkers();
    }

}

//logic to scout at X supply until we find the enemy
//will be made more complicated
void Decider::setScouting(){

    const int scoutStart = 16;

    if(gs->mapPtr->enemyMain == nullptr){
            if(BWAPI::Broodwar->self()->supplyUsed() > scoutStart){
                gs->isScouting = true;
            }
    }else(gs->isScouting = false); //this is if we found the enemy

}

//have to run AFTER updated owned bases
void Decider::calculateWorkers(){
    const BWAPI::Unitset units = BWAPI::Broodwar->self()->getUnits();
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    gs->workerCount = Tools::CountUnitsOfType(worker, units);

    //update mineral count and assimilator count per base
    //based on this infer max efficient worker count
    int workerMax = 0;
    for(std::pair<baseEconomy, const BWEM::Base *>p : gs->workerTotals){
        if(p.first.isOccupied){
            p.first.minCount = p.second->Minerals().size(); //shooting for two workers per min patch
            workerMax += (p.first.minCount * 2);
            size_t assimilatorCount = 0;
            //check if assimilator is done on geysers in base
            for(auto geyser : p.second->Geysers()){
                BWAPI::TilePosition tpos = BWAPI::TilePosition(geyser->Pos());
                for(auto unit : BWAPI::Broodwar->getUnitsOnTile(tpos)){
                    if(unit->getType().isRefinery() && unit->isCompleted() ){
                        assimilatorCount++;
                        workerMax += 4;
                    }
                };
            };
            p.first.assimilatorCount = assimilatorCount;
        }
    }
    //based on above store worker max
    gs->workerMax = workerMax;

    int assignedWorkers = 0;

    //assign min workers to each base based on priority
    for (size_t i = 0; i < gs->activeBaseCount; i++){
        for(std::pair<baseEconomy, const BWEM::Base *>p : gs->workerTotals){
            if(!p.first.isOccupied) continue;
            if(p.first.basePriority == i){
                p.first.onMin = p.first.minCount;
                assignedWorkers += p.first.onMin;
                if (assignedWorkers >= gs->workerCount) break;
                p.first.onGas = p.first.assimilatorCount * 3;
                assignedWorkers += p.first.onGas;
                break;
            }
        }
    }
    
    //assign additional workers until out
    while(assignedWorkers >= gs->workerCount){
        for (size_t i = 0; i < gs->activeBaseCount; i++){
            for(std::pair<baseEconomy, const BWEM::Base *>p : gs->workerTotals){
                if(!p.first.isOccupied) continue;
                if(p.first.basePriority == i){
                    p.first.onMin++;
                    assignedWorkers++;
                    if (assignedWorkers >= gs->workerCount) break;
                    //max on 4 on gas per assimilator
                    if(p.first.onGas < (4 * p.first.assimilatorCount)){
                        p.first.onGas++;
                        assignedWorkers++;                               
                    }
                    break;
                }
            }
        }
    }

}

void Decider::updateOwnedBases(){
    gs->activeBaseCount = 0; //reset count of occupied bases to zero temporarily before we count them

    for(std::pair<baseEconomy, const BWEM::Base *> pair : gs->workerTotals){
        
        for(auto unit : BWAPI::Broodwar->getUnitsOnTile(pair.second->Location()) ){ //check each base for completed CC
            pair.first.isOccupied = false;
            if( unit->getType().isResourceDepot() && unit->isCompleted() && (unit->getPlayer() == BWAPI::Broodwar->self()) ){   
                pair.first.isOccupied = true;
                gs->activeBaseCount++;
                if(pair.second == gs->mapPtr->myMain){ pair.first.basePriority = 0;}
                    else if(pair.second == gs->mapPtr->myNatural){ pair.first.basePriority = 1;}
                        else (pair.first.basePriority = 2);
                break; //skip to next base if we confirm CC
            }
        }
    }
}
