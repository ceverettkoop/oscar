#include "StarterBot.h"
#include <BWAPI.h>
#include <fstream>
#include "Tools.h"

//defined in main
StarterBot* bot;

void InitialBuildOrder::nextStep(int dblSupplyCount, int* targetCount){

    int supplyCount = dblSupplyCount; //ibo was doubled already whoops
        
    //assume constant droning unless flagged no or switched below
    bool droning = true;
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    BWAPI::UnitType pylon = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    //cut drones if next one will eff pylon
    if(ibo_unitType[curStep+1] == pylon && ( (supplyCount + 2) >= ibo_supplyCount[curStep+1] ) ){
        if( BWAPI::Broodwar->self()->minerals() < 150)  droning = false;
    }

    if(droning){ 
        bot->bq.addEntryNow(1, worker);
    }

    //also build prescribed building or unit but only one per step 
    for (int i = 0; i < ibo_supplyCount.size(); ++i){
        if (supplyCount >= ibo_supplyCount[i]){
            curStep = i;
            if (lastStep < curStep){ //if new step 
                bot->bq.addEntryNow(1, ibo_unitType[i]); //queue one up
                if (supplyCount >= ibo_supplyCount.back()) isFinished = true;
                *targetCount = ibo_supplyCount[i];
                lastStep = curStep; //reset step check
                return;
            }
        }
    } 
}

//take a file of integers seperated by space or newline of the format:
//supplycount unittype \n supplycount unittype etc etc
//returns -1 on failure otherwise returns # of steps found in build order
int InitialBuildOrder::load_ibo(char* path){

    std::ifstream infile (path);
    std::string instring;
    char inchar;
    int inint;
    bool isCount = true;
    int i = 0;
    
    if(infile.is_open()){
        while (infile.good()){
            inchar = infile.get();
            if (inchar == '#') break; //indicates end of ibo at #
            if( inchar != ' ' && inchar != '\n'){
                instring += inchar;
            }else{ //presumably we have an int
                inint = std::stoi(instring, NULL);
                if(isCount){
                    ibo_supplyCount.push_back(inint);
                    isCount = false;
                    instring.clear();
                }else{
                    ibo_unitType.push_back(BWAPI::UnitType(inint));
                    isCount = true;
                    i++;
                    instring.clear();
                }
            }
        }
        
    }else {
        fprintf(stderr, "Failed to load input file\n");
        stepCount = 0;
        return -1;
    }

    stepCount = i;

    //doubling supply values to match API!
    for (size_t n = 0; n < ibo_supplyCount.size(); n++){
        ibo_supplyCount[n] = (ibo_supplyCount[n]) * 2;
    }
    

    return i;

}

int InitialBuildOrder::DesiredCountAlreadyBuilt(BWAPI::UnitType type){
    int currentStep = 0;
    int countDesired = 0;
    for(int i = 0; i < ibo_supplyCount.size(); i++){
        if(ibo_supplyCount[i] <=  BWAPI::Broodwar->self()->supplyUsed())
            currentStep = i;
    }

    for(int i = 0; i <= currentStep; i++){
        if(ibo_unitType[i] == type)
            countDesired++;
    }

    return countDesired;

}

//update queue, taking into account of last attempt to build queued unit
void BuildQueue::updateQueue(){
    int targetCount = 0;
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    if(bot->ibo.isFinished) onIbo = false; //only leaves ibo if over; will add other conditions
    
    //logic for initial build order 
    if(onIbo){
        bot->ibo.nextStep(BWAPI::Broodwar->self()->supplyUsed(), &targetCount);

    }else{ //past ibo behavior

        BWAPI::UnitType pylon = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
        int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
        int droneCount = Tools::CountUnitsOfType(worker, BWAPI::Broodwar->self()->getUnits());

        //PYLON QUEUE (should work for other supply too)
        //are we supply blocked add 1 pylon to queue
        int totalSupply = Tools::GetTotalSupply(true); //incl under construction
        if( (supplyUsed + 4) >= totalSupply){
            addEntryNow(1, pylon);
        }
        
        //assimilator test
            addEntryTotal(1, BWAPI::UnitTypes::Protoss_Assimilator);

        //try to build one dragoon to see how much it fucks up
            addEntryTotal(4, BWAPI::UnitTypes::Protoss_Dragoon);

        //DRONE QUEUE
            addEntryTotal(40, worker);

        //ZEALOT QUEUE
            addEntryTotal(10, BWAPI::UnitTypes::Protoss_Zealot);

    }

        
//skippin goal nonsense for now      
/*        BuildGoal goal = GetGoal(supplyUsed, droneCount, baseCount);
            
        switch(goal){ //goal busted maybe will get canned

            case BUILD_ZEALOTS:
                bool buildingZealot = false; 
                for (size_t i = 0; i < next.size(); i++){
                    if (next[i].type == BWAPI::UnitTypes::Protoss_Zealot ) buildingZealot = true;
                }
                if (!buildingZealot) next.push_back(QueueEntry(BWAPI::UnitTypes::Protoss_Zealot));
                break;
            }
            
        }
*/

}

//this does nothing rn
BuildGoal BuildQueue::GetGoal(int supplyUsed, int droneCount, int baseCount){

    int boSupply = supplyUsed / 2;
    //inline for now will append to IBO later;
    if(boSupply < 30){
        return BUILD_ZEALOTS;
    }else if (baseCount < 2 ){
        return BUILD_ZEALOTS; //this will be expand later
    }else 
        return BUILD_ZEALOTS;

}

void BuildQueue::addEntryNow(int count, BWAPI::UnitType type){

    bool typeExists = false;
    
    //if type already in queue reset count wanted and built
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].type){
            typeExists = true; 
            next[i].countWantedNow = count;
            next[i].countBuiltNow = 0;            
        }     
    }

    //else create it
    if (!typeExists){
        next.push_back(QueueEntry());
        next.back().type = type;
        next.back().countWantedNow = count;
        next.back().countBuiltNow = 0;
    }
}

void BuildQueue::addEntryTotal(int count, BWAPI::UnitType type){

   bool typeExists = false;
    
    //if type already in queue reset count wanted and built
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].type){
            typeExists = true; 
            next[i].countWantedTotal = count;
            next[i].countBuiltTotal=  Tools::CountUnitsOfType(type, BWAPI::Broodwar->self()->getUnits() );           
        }     
    }    

    if(!typeExists){
        next.push_back(QueueEntry());
        next.back().type = type;
        next.back().countWantedTotal = count;
        next.back().countBuiltTotal = Tools::CountUnitsOfType(type, BWAPI::Broodwar->self()->getUnits() ); //init
    }    
}

int BuildQueue::updateQty(int index){

    //now takes precedence

    if(next[index].countWantedNow != -1){
        int nowBuild = next[index].countWantedNow - next[index].countBuiltNow;
        if (nowBuild > 0){
            next[index].buildQty = nowBuild;
            return nowBuild;
        }  
    }

    //if relevant this is where we check countBuiltTotal
    next[index].countBuiltTotal = Tools::CountUnitsOfType(next[index].type, BWAPI::Broodwar->self()->getUnits());

    if(next[index].countWantedTotal != -1){
        int totalBuild = next[index].countWantedTotal - next[index].countBuiltTotal;
        if (totalBuild > 0) {
            next[index].buildQty = totalBuild;
            return totalBuild;
        }
    }

    next[index].buildQty = 0;
    return 0;
}

void BuildQueue::rmEntry(BWAPI::UnitType type){

    //not sure why you would need to but this will kill off a bq entry
    bool typeExists = false;
    int index = -1;
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].type) typeExists = true;
        index = i;
    }

    if(typeExists) next.erase(next.begin() + index);

}

//send queue entry of specified type to front of the list
void BuildQueue::entryToFront(BWAPI::UnitType type){

    bool typeExists = false;
    int index = -1;
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].type) typeExists = true;
        index = i;
    }

    if(typeExists) std::rotate(next.begin(), (next.begin() + index), next.end());

}

//won't work for lurkers, should for all else; should check if we need prereqs at all before calling this
BWAPI::UnitType BuildQueue::queueNextPrereq(BWAPI::UnitType type){

    if(type == BWAPI::UnitTypes::Protoss_Probe || type == BWAPI::UnitTypes::Protoss_Nexus){
        return BWAPI::UnitTypes::None;
    }

    const std::map<BWAPI::UnitType, int> reqs = type.requiredUnits();

    for (auto const &p : reqs){
        BWAPI::UnitType reqType =  p.first;
        if(!Tools::CountUnitsOfType(reqType, BWAPI::Broodwar->self()->getUnits())){
            addEntryNow(1, reqType);
            return reqType; //only building the first one
        }
    } 

    //if we didn't find any reqs
    return BWAPI::UnitTypes::None;
}

