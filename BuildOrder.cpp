#include <BuildOrder.h>
#include <BWAPI.h>
#include <fstream>


//constructor
InitialBuildOrder::InitialBuildOrder(){ 

}

bool InitialBuildOrder::isFinished(int supplyCount){
    
    return (supplyCount > ibo_supplyCount.back());

}

BWAPI::UnitType InitialBuildOrder::nextStep(int supplyCount, int* targetCount){

    for (size_t i = 0; i < ibo_supplyCount.size(); i++){
        if (supplyCount >= ibo_supplyCount[i]){
            if(supplyCount == ibo_supplyCount[i]){
                *targetCount = ibo_supplyCount[i];
                return ibo_unitType[i];
            }else{
                *targetCount = ibo_supplyCount[i];
                return BWAPI::Broodwar->self()->getRace().getWorker();
            }
            
        }
    }

}

//take a file of integers seperated by space or newline of the format:
//supplycount unittype \n supplycount unittype etc etc
//returns -1 on failure otherwise returns # of steps found in build order
int InitialBuildOrder::load_ibo(std::string* path){

    std::ifstream infile (*path);
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
                    ibo_supplyCount[i] = inint;
                    isCount = false;
                    instring.clear();
                }else{
                    ibo_unitType[i] = BWAPI::UnitType(inint);
                    isCount = true;
                    i++;
                    instring.clear();
                }
            }
        }
        
    }else {
        fprintf(stderr, "Failed to load input file\n");
        return -1;
    }

    stepCount = i;
    return i;

}