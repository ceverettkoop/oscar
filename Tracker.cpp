#include "StarterBot.h"
#include "Tools.h"
#include <BWAPI.h>
#include <vector>


//updates tracker AND initiates relevant commands based on result
void Tracker::processTracker(){

    for(auto& entry: BuilderList){
        CommandResult result = didBuilderSucceed(entry.first, entry.second);
        if (result == FAIL_AND_RETRY) bot->bq.addEntryNow(1, entry.second.buildType);
    }
    
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
            untrackBuilder(key);
            return FAIL_AND_RETRY;
        }else{
            return SUCCESS;}
    }else{
        untrackBuilder(key);
        return ONGOING;
    }

}

void Tracker::untrackBuilder(int key){

    BuilderList.erase(key);

}