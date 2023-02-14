#pragma once

#include "StarterBot.h"
#include <BWAPI.h>
#include <vector>

//updates tracker AND initiates relevant commands based on result
void Tracker::processTracker(){


    
}

 //returns key to tracked builder
int Tracker::trackBuilder(BWAPI::Unit unit, BWAPI::UnitType buildType){
    
    int key = lastKey + 1;

    Builder input;
    input.unit = unit;
    input.buildType = buildType;
    BuilderList.insert({key, input});

    lastKey = key;

}

//if success builder is auto removed from tracking
CommandResult Tracker::didBuilderSucceed(int key){

    BWAPI::Unit found = BuilderList.at(key).unit;
    BWAPI::UnitCommand command = found->getLastCommand();



}