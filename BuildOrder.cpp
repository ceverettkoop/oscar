#include "StarterBot.h"
#include <BWAPI.h>
#include <fstream>
#include "Tools.h"


BWAPI::UnitType InitialBuildOrder::nextStep(int supplyCount, int* targetCount){

    for (size_t i = 0; i < ibo_supplyCount.size(); i++){
            if (supplyCount <= ibo_supplyCount[i]){
            if(supplyCount == ibo_supplyCount[i]){
                if (supplyCount >= ibo_supplyCount.back()) isFinished = true;
                *targetCount = ibo_supplyCount[i];
                return ibo_unitType[i];
            }else{
                //double supply count will be broken TODO
                *targetCount = ibo_supplyCount[i];
                BWAPI::UnitType test = BWAPI::Broodwar->self()->getRace().getWorker();
                return test;
            } 
        }
    }

    fprintf(stderr, "Something failed in IBO reader\n");
    return BWAPI::Broodwar->self()->getRace().getWorker();    
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
        BWAPI::UnitType rec = bot->ibo.nextStep(BWAPI::Broodwar->self()->supplyUsed(), &targetCount);
        
        if(!rec.isBuilding()){ // if we have a unit pass it
            next[0].type = rec;
            
        }else{ //otherwise check if we have built as many as prescribed already per ibo
            const BWAPI::Unitset& units = BWAPI::Broodwar->self()->getUnits();
            int builtCount = 0;
            for (auto& unit : units){ 
                if(unit->getType() == rec) builtCount++;
            }
            if (builtCount < bot->ibo.DesiredCountAlreadyBuilt(rec)){
                next[0].type = rec;
                return;
            }else{
                next[0].type = worker;
                return;
            }
        }
    }else{ //past ibo behavior now using more queue slots

        BWAPI::UnitType pylon = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
        int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
        int droneCount = Tools::CountUnitsOfType(worker, BWAPI::Broodwar->self()->getUnits());

        //PYLON QUEUE (should work for other supply too)
        //are we supply blocked add 1 pylon to queue
        int totalSupply = Tools::GetTotalSupply(true); //incl under construction
        if( (supplyUsed + 4) >= totalSupply){
            addEntryNow(1, pylon);
        }

        //DRONE QUEUE
            addEntryTotal(40, worker);

        //ZEALOT QUEUE
            addEntryTotal(40, BWAPI::UnitTypes::Protoss_Zealot);

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


