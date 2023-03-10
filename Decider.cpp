#include "Decider.h"
#include "OscarMap.h"
#include "GameState.h"
#include "Tools.h"
#include <map>


void Decider::onStart(){

    updateOwnedBases();
    calculateWorkers();

}


void Decider::onFrame(){

    //decide if we are scouting
    setScouting();

    //placeholder expansion logic
    //if over 20 workers and nexus not already in the queue flip it on
    if(gs->workerCount > 12){
        gs->basesDesired = 2;
    }

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

    //if it's not a worker or a building; assign it a combat group
    if(!unit->getType().isWorker() && !unit->getType().isBuilding());

}

void Decider::onUnitDestroy(BWAPI::Unit unit){

    //if it's a nexus also update our count of what bases are ours
    if(unit->getType().isResourceDepot()){ 
        updateOwnedBases();
    }

    //we're only going to recalculate worker assignments if it's a nexus or a worker OR refinery OR if min depleted
    if(unit->getType().isWorker() ||  unit->getType().isResourceDepot() || unit->getType().isRefinery() ||
        unit->getType().isMineralField()){
        calculateWorkers();
    }

}

//logic to scout at X supply until we find the enemy
//will be made more complicated
void Decider::setScouting(){

    const int scoutStart = 18;

    if(gs->mapPtr->enemyMain == nullptr){
            if(BWAPI::Broodwar->self()->supplyUsed() > scoutStart){
                gs->isScouting = true;
            }
    }else(gs->isScouting = false); //this is if we found the enemy

}

//set relevant variables to determinign worker count per base
//this function should be renamed!
void Decider::calculateWorkers(){
    const BWAPI::Unitset units = BWAPI::Broodwar->self()->getUnits();
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    gs->workerCount = Tools::CountUnitsOfType(worker, units);

    //update mineral count and assimilator count per base
    //based on this infer max efficient worker count
    int workerMax = 0;

    for(auto &p : gs->ownedBaseTracker){

        //update min count
        p.second.minCount = p.second.base->Minerals().size();
        workerMax += (p.second.minCount * 2); //update max 2 per min

        size_t assimilatorCount = 0;
        //check if assimilator is done on geysers in base
        for(auto geyser : p.second.base->Geysers()){
            BWAPI::TilePosition tpos = BWAPI::TilePosition(geyser->Pos());
            for(auto unit : BWAPI::Broodwar->getUnitsOnTile(tpos)){
                if(unit->getType().isRefinery() && unit->isCompleted() ){
                    assimilatorCount++;
                    workerMax += 4;
                }
            };
        };
        //update assimilator count (workermax augmented above)
        p.second.assimilatorCount = assimilatorCount;

    }

    //adding 2 to worker max for builders etc
    gs->workerMax = workerMax + 2;

    //ALL OTHER DECISIONS NOW MADE AT ASSIGN WORKERS FUNCTION

}

void Decider::updateOwnedBases(){
    gs->activeBaseCount = 0; //reset count of occupied bases to zero temporarily before we count them
    int key = -1;

    //for all bases
    for(auto &base : gs->mapPtr->allBases){ 

        bool owned = false;
         //check each base for completed CC that we own and if it's ours mark it
        for(auto unit : BWAPI::Broodwar->getUnitsOnTile(base->Location()) ){
            if( unit->getType().isResourceDepot() && unit->isCompleted() && (unit->getPlayer() == BWAPI::Broodwar->self()) ){   
                
                gs->activeBaseCount++;
                owned = true;

                //if we have never determined our main base this will be set on first run
                if(gs->mapPtr->myMain == nullptr){
                    gs->mapPtr->myMain = base;
                }
                //ditto nat
                if(gs->mapPtr->myNatural == nullptr){
                    gs->mapPtr->assignNatural();
                }

                //unique key per base
                //0 for main; 1 for nat; other bases pseudo unique value based on distance from main
                if (base == gs->mapPtr->myMain){
                    key = 0;
                }else if (base == gs->mapPtr->myNatural){
                    key = 1;
                }else{ //generate key based on distance from main + distance from nat (SHOULD be unique)
                    key = 100 + base->Center().getApproxDistance(gs->mapPtr->myMain->Center()) +
                        base->Center().getApproxDistance(gs->mapPtr->myNatural->Center());
                }

                baseEconomy newBase;
                newBase.base = base;

                //store our base on our econTracker if not already there
                gs->ownedBaseTracker.insert({key, newBase});
            }
        }

        if (!owned){
             //finally if we don't own this base; check if its in our owned BaseTracker and if it is delete the entry
            if (gs->ownedBaseTracker.size() == 0) continue; //error check?
            for(auto it = gs->ownedBaseTracker.begin(); it != gs->ownedBaseTracker.end(); ++it){
                if (base == it->second.base){
                    gs->ownedBaseTracker.erase(it);
                }
            }
        }  
    }
}