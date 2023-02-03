#include <BuildOrder.h>
#include <BWAPI.h>


//constructor
InitialBuildOrder::InitialBuildOrder(){ 

}

bool InitialBuildOrder::isFinished(int supplyCount){
    
    return (supplyCount > ibo_supplyCount.back());

}

BWAPI::UnitTypes::Enum::Enum InitialBuildOrder::nextStep(int supplyCount, int* targetCount){

    for (size_t i = 0; i < ibo_supplyCount.back(); i++){
        if (supplyCount >= ibo_supplyCount[i]){
            *targetCount = ibo_supplyCount[i];
            return ibo_unitType[i];
        }
    }
    

}


int InitialBuildOrder::load_ibo(std::string* path){

}