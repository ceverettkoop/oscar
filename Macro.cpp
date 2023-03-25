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
            }
                
            for(auto& base : gs->mapPtr->mainBases){
                if(base->Location() == enemyLocation){
                    gs->mapPtr->enemyMain = base;
                    gs->isScouting = false;    //end scouting now
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
    //logic for worker to flee enemy or fight etc handled in micro which procs after macro
    //this should only impact idle workers or workers assigned to mining so in theory microing workers unaffected

    BWAPI::UnitType type = BWAPI::Broodwar->self()->getRace().getWorker();

    //global to all bases
    BWAPI::Unitset idleWorkers;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if (unit->getType() == type && unit->isCompleted() && unit->isIdle()){
            idleWorkers.insert(unit);
        }
    }

    BWAPI::Unit worker;
    BWAPI::Unitset localMiners;

    //first count at all occupied bases
    for(auto &p : gs->ownedBases){
        p.second.minerCount = countMinersInBase(p.second.base);
        p.second.gasMinerCount = countGasCollectorsInBase(p.second.base);
    }

    //next loop, assign wokers
    for(auto it = gs->ownedBases.begin(); it->first != gs->ownedBases.rbegin()->first; ++it){
        
        auto &p = *it;
        //reset localMiners
        localMiners.clear();

        //create set of localMiners for this base
        for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
            BWAPI::Unit target = unit->getOrderTarget();
            if (unit->getType() == type && unit->isCompleted() && unit->isGatheringMinerals()){ 
                if (target != nullptr){
                    if((unit->getOrderTarget()->getDistance(p.second.base->Center()) < 640 )){
                        localMiners.insert(unit);
                    }
                }
            }
        }

        //GAS 
        //ONLY DEALING W ONE GEYSER FOR NOW
        auto geyser = p.second.base->Geysers().front();

        //while we have less gas collectors than needed
        while(p.second.gasMinerCount < (p.second.assimilatorCount * 3)){
            
            //try to assign an idle worker first then steal a local miner
            if(idleWorkers.size() > 0){
                worker =  *idleWorkers.begin();
                Tools::SmartRightClick(worker, geyser->Unit());
                p.second.gasMinerCount++;
                idleWorkers.erase(idleWorkers.begin());
            }else if(localMiners.size() > 0){
                    worker =  *localMiners.begin();
                    Tools::SmartRightClick(worker, geyser->Unit());
                    p.second.gasMinerCount++;
                    localMiners.erase(localMiners.begin());
                    p.second.minerCount--;
                }else break; //if both sets empty give up
        }

        //MINERALS
        //while we have less min collectors than one per min
        while(p.second.minerCount < p.second.minCount ){

            //first try to assign an idle worker
            if(idleWorkers.size() > 0){
                worker =  *idleWorkers.begin();

                //find best mineral, this will be a function later
                BWAPI::Unit targetMin;
                for(auto min : p.second.base->Minerals()){
                    targetMin = min->Unit();
                    if(!min->Unit()->isBeingGathered()) break;
                }

                Tools::SmartRightClick(worker, targetMin);
                p.second.minerCount++;
                idleWorkers.erase(idleWorkers.begin());
        
            }else break; //if out then give up
        }

        //now if miner count is GREATER THAN minimum AND next base is less than minimum send some over
        //if this is the last base then this obviously doesn't occur
        
        if (it->first != gs->ownedBases.rbegin()->first){
            auto nextIt = std::next(it, 1);
            auto &nextP = *nextIt;
            while(p.second.minerCount > p.second.minCount && nextP.second.minerCount < nextP.second.minCount ){
                worker = *localMiners.begin();
                worker->move(nextP.second.base->Center()); 
                nextP.second.minerCount++;
                p.second.minerCount--;
            }
        }
        
    }

    //if after all that we still have idle workers send them to mine at bases until maxxed out everywhere
    while(idleWorkers.size() > 0){
        
        const BWEM::Base *base = nullptr;
        worker =  *idleWorkers.begin();

        //select first base unless maxed, then advance
        for(auto &p : gs->ownedBases){
            if(p.second.minerCount >= (p.second.minCount*2)){ //if mins maxxed
                if(p.first != gs->ownedBases.rbegin()->first){ //and not the last base
                    continue; //go to next base
                }
            }
            base = p.second.base;
            break;
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


//for speed just counting a miner that is within 8 build tiles of center of base (not ideal)
int MacroManager::countMinersInBase(const BWEM::Base *base){

    int count = 0;

    BWAPI::UnitType type = BWAPI::Broodwar->self()->getRace().getWorker();
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if(unit->getType() = type && unit->isGatheringMinerals()){
            if( unit->getDistance(base->Center()) < 256 ){
                count++;
            }
        }
    }

    return count;

}

//for speed just counting a miner that is within 8 build tiles of center of base (not ideal)
int MacroManager::countGasCollectorsInBase(const BWEM::Base *base){

    int count = 0;

    BWAPI::UnitType type = BWAPI::Broodwar->self()->getRace().getWorker();
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        if(unit->getType() = type && unit->isGatheringGas()){
            if( unit->getDistance(base->Center()) < 256 ){
                count++;
            }
        }
    }

    return count;

}