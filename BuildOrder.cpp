#include "StarterBot.h"
#include <BWAPI.h>
#include <fstream>


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
void BuildQueue::updateQueue(BuildResult lastResult){
    int targetCount = 0;
    if(bot->ibo.isFinished) onIbo = false; //only leaves ibo if over; will add other conditions
    
    //logic for initial build order 
    if(onIbo){
        BWAPI::UnitType rec = bot->ibo.nextStep(BWAPI::Broodwar->self()->supplyUsed(), &targetCount);
        
        if(!rec.isBuilding()){ // if we have a unit pass it
            next = rec;
        }else{ //otherwise check if we have built as many as prescribed already per ibo
            const BWAPI::Unitset& units = BWAPI::Broodwar->self()->getUnits();
            int builtCount = 0;
            for (auto& unit : units){ 
                if(unit->getType() == rec) builtCount++;
            }
            if (builtCount < bot->ibo.DesiredCountAlreadyBuilt(rec)){
                next = rec;
                return;
            }else{
                next = BWAPI::Broodwar->self()->getRace().getWorker();
            }
            

        }

        
    }

}

