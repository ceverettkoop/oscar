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

    //we're only going to recalculate worker assignments if it's a nexus or a worker
    if(unit->getType().isWorker() ||  unit->getType().isResourceDepot()){
        calculateWorkers();
    }

    //if it's a nexus also update our count of what bases are ours
    if(unit->getType().isResourceDepot()){ 
        updateOwnedBases();
    }

}

void Decider::onUnitDestroy(BWAPI::Unit unit){

    //we're only going to recalculate worker assignments if it's a nexus or a worker
    if(unit->getType().isWorker() ||  unit->getType().isResourceDepot()){
        calculateWorkers();
    }

    //if it's a nexus also update our count of what bases are ours
    if(unit->getType().isResourceDepot()){ 
        updateOwnedBases();
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

void Decider::calculateWorkers(){
    const BWAPI::Unitset units = BWAPI::Broodwar->self()->getUnits();
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    gs->workerCount = Tools::CountUnitsOfType(worker, units);

    for(std::pair<baseEconomy, const BWEM::Base *>p : gs->workerTotals){
        if(p.first.isOccupied){
            p.first.idealOnMin =  2 * p.second->Minerals().size(); //shooting for two workers per min patch
            //check if assimilator is done on geysers in base
            size_t refineryCount = 0;
            
            p.first.idealOnGas = 4 * p.second->Geysers().size(); //four per gas

        }
    }


}

void Decider::updateOwnedBases(){


}
