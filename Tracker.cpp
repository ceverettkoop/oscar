#include "StarterBot.h"
#include "Tools.h"
#include <BWAPI.h>
#include <vector>


//updates tracker AND initiates relevant commands based on result
void Tracker::processTracker(){
    
    //builders
    std::vector<int> completedBuilders;

    for(auto& entry: BuilderList){
        CommandResult result = didBuilderSucceed(entry.first, entry.second);
        if (result == FAIL_AND_RETRY) bot->bq.addEntryNow(1, entry.second.buildType);
        if (result != ONGOING) completedBuilders.push_back(entry.first);
    }

    //delete former builders
    for(auto& key: completedBuilders){
        BuilderList.erase(key);
    }

    //TODO MOVE THIS ELSEWHERE; for now decision if we are scouting is here
    if(bot->gs.enemyLocation == BWAPI::TilePositions::Unknown){
        if(!bot->gs.scouting){
            if(BWAPI::Broodwar->self()->supplyUsed() > 16){
                bot->gs.scouting = true;
            }
        }
    }else(bot->gs.scouting = false); //this is if we found the enemy

    //assign scout if needed
    if(bot->gs.scouting){
        if(scout == nullptr || !scout->exists()){ //if scout not assigned or dead
            scout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
        }
    }else scout = nullptr; //if not scouting release any unit from being called a scout
    
    
}

 //returns key to tracked builder
int Tracker::trackBuilder(BWAPI::Unit unit, BWAPI::UnitType buildType){
    
    int key = lastKey + 1;

    Builder input;
    input.unit = unit;
    input.buildType = buildType;
    input.initBuildCount = Tools::CountUnitsOfType(buildType,BWAPI::Broodwar->self()->getUnits());
    BuilderList.insert({key, input});
    lastKey = key;

    return key;

}

//if success builder is auto removed from tracking
CommandResult Tracker::didBuilderSucceed(int key, Builder found){

    //assuming as soon as it's done building it will be issued a new command

    //BWAPI::UnitCommand command = found.unit->getLastCommand();

    //assume buildtype matches unit is on the way
    if(found.unit->getBuildType() != found.buildType){
        int newCount = Tools::CountUnitsOfType(found.buildType,BWAPI::Broodwar->self()->getUnits());
        if (newCount <= found.initBuildCount){ 
            return FAIL_AND_RETRY;
        }else{
            return SUCCESS;}
    }else{
        return ONGOING;
    }

}
