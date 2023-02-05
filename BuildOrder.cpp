#include "StarterBot.h"
#include <BWAPI.h>
#include <fstream>


BWAPI::UnitType InitialBuildOrder::nextStep(int supplyCount, int* targetCount){

    for (size_t i = 0; i < ibo_supplyCount.size(); i++){
        if (supplyCount >= ibo_supplyCount[i]){
            if(supplyCount == ibo_supplyCount[i]){
                if (supplyCount == ibo_supplyCount.back()) isFinished = true;
                *targetCount = ibo_supplyCount[i];
                return ibo_unitType[i];
            }else{
                *targetCount = ibo_supplyCount[i];
                return BWAPI::Broodwar->self()->getRace().getWorker();
            } 
        }
    }

    fprintf(stderr, "Something failed in IBO reader");
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
    return i;

}

//update queue, taking into account of last attempt to build queued unit
void BuildQueue::updateQueue(BuildResult lastResult){
    int targetCount = 0;
    //if(bot->ibo.isFinished) onIbo = false; //only leaves ibo if over; will add other conditions
    
    if(onIbo){
        next = bot->ibo.nextStep(BWAPI::Broodwar->self()->supplyUsed(), &targetCount);
    }

}
