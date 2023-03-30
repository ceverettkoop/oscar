//Really manages everything to do with building buildings or training units
#ifndef WIN32 //if compiling not on windows
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <fstream>

#include "BuildOrder.h"
#include "OscarMap.h"
#include "Tools.h"
#include <cstring>

//first all BQ functions; then IBO (should be folded in later); then tracker
//Load ibo from file, choose based on race and opponent
void BuildQueue::onStart(){
    
    ibo.bq = this;
    ibo.load_ibo(getIboPath());
    track.bq = this;
}

std::string BuildQueue::getIboPath(){

    char path[1028];
    
    //get current working directory
    #ifndef WIN32
    getcwd(path, 400);
    #else
    _getcwd(path, 200);
    #endif

    //append read directory
    strcat(path,"/bwapi-data/read");

    //append own race
    switch (BWAPI::Broodwar->self()->getRace()){
    case BWAPI::Races::Zerg :
        strcat(path, "/zerg");
        break;

    case BWAPI::Races::Protoss :
        strcat(path, "/protoss");
        break;

    case BWAPI::Races::Terran :
        strcat(path, "/terran");
        break;
    }

    //append enemy race
    switch (BWAPI::Broodwar->enemy()->getRace()){
    case BWAPI::Races::Zerg :
        strcat(path, "/vzerg");
        break;

    case BWAPI::Races::Protoss :
        strcat(path, "/vprotoss");
        break;

    case BWAPI::Races::Terran :
        strcat(path, "/vterran");
        break;

    }

    //TODO PICK IBOS FROM ALL IN DIRECTORY
    //FOR NOW WE ARE JUST PICKING ONE LABELED DEFAULT
    strcat(path, "/default");

    fprintf(stderr,"IBO path is %s\n", path);

    std::string spath = path;
    return spath;

}


//Iterate through entries in building queue and build what we are supposed to build
void BuildQueue::onFrame(){

    updateQueue();

    //iterate through every item in queue
    for (int i = 0; i < next.size(); i++){
        
        if(next[i].isUpgrade){
            handleUpgradeEntry(next[i], i);
        }else{
            handleUnitEntry(next[i], i);
        }

    }

    //tracker updates
    track.onFrame();
    clearEmptyEntries();
    
    return;
    
}

void BuildQueue::handleUnitEntry(QueueEntry& entry, int index){

    BWAPI::UnitType nextUnit = entry.type;

    int countToBuild = updateQty(index); //check how many to build
    if (countToBuild < 1 ) return;

    //checks for gas min and supply against what we have as well as what we have already set aside
    bool canAfford = ( nextUnit.mineralPrice() <= (BWAPI::Broodwar->self()->minerals() - minCommited) && 
        nextUnit.gasPrice() <= (BWAPI::Broodwar->self()->minerals() - gasCommited) &&
        nextUnit.supplyRequired() <= (BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed() - supplyCommited)  );

    if(canAfford){
        if(nextUnit.isBuilding()){
            bool built = false;
            for (int n = 0; n < countToBuild; ++n){
                BWAPI::Unit foundBuilder;
                built = BuildBuilding(nextUnit, &foundBuilder, determineLocation(nextUnit));
                if(built){
                    next[index].countBuiltNow++; //tell queue we did it
                    //if it was built by a unit go track to make sure it happens
                    if(!foundBuilder->getType().isBuilding()){
                        track.trackBuilder(foundBuilder, nextUnit); //follow builder to make sure we actually build it
                        ///set aside money
                        minCommited += nextUnit.mineralPrice();
                        gasCommited += nextUnit.gasPrice();
                        supplyCommited += nextUnit.supplyRequired(); 
                    }
                }
                
            }
        }else{
            for (int n = 0; n < countToBuild; ++n){
                if(TrainUnit(nextUnit) == QUEUED) next[index].countBuiltNow++;
            //should not need to set aside money here because it's either spent or it's not
            }
        }
    }

    //somehow i think putting this after the build commands helps idk
    //adds prereqs to queue if we have need
    if (!onIbo) queueNextPrereq(nextUnit);

}

void BuildQueue::handleUpgradeEntry(QueueEntry& entry, int index){

    BWAPI::UpgradeType upgrade = entry.upType;
    bool success = false;

    //checks for gas min and supply against what we have as well as what we have already set aside
    bool canAfford = ( upgrade.mineralPrice() <= (BWAPI::Broodwar->self()->minerals() - minCommited) && 
        upgrade.gasPrice() <= (BWAPI::Broodwar->self()->minerals() - gasCommited));

    if(canAfford){


        BWAPI::UnitType rType= upgrade.whatUpgrades();
        BWAPI::Unitset researchers = Tools::GetUnitSetofType(rType);

        for(auto unit : researchers){
            if(!unit->isResearching()){
                success = unit->upgrade(upgrade);
                if(success) break;
            }
        }

        if(success) rmEntry(upgrade, entry.upgradeLevel);

    }

}


//THIS IS CURRENTLY WHERE ALL STRATEGY IS INLINED BEYOND IBO

//update queue, taking into account of last attempt to build queued unit
void BuildQueue::updateQueue(){
    int targetCount = 0;
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    if(ibo.isFinished) onIbo = false; //only leaves ibo if over; will add other conditions
    
    //logic for initial build order 
    if(onIbo){
        ibo.nextStep(BWAPI::Broodwar->self()->supplyUsed(), &targetCount);

    }else{ //past ibo behavior

        BWAPI::UnitType pylon = BWAPI::Broodwar->self()->getRace().getSupplyProvider();
        int supplyUsed = BWAPI::Broodwar->self()->supplyUsed();
        int droneCount = Tools::CountUnitsOfType(worker, BWAPI::Broodwar->self()->getUnits());

        //PYLON QUEUE (should work for other supply too)
        //are we supply blocked add 1 pylon to queue
        int totalSupply = Tools::GetTotalSupply(true); //incl under construction
        if( (supplyUsed + 4) >= totalSupply){
            replaceEntryNow(1, pylon);
        }

        //DRONE QUEUE set to workermax
            addEntryTotal(gs->workerMax, worker);


        //Try to expand if told to
            if(gs->basesDesired > gs->activeBaseCount){
                addEntryTotal(gs->basesDesired , BWAPI::BroodwarPtr->self()->getRace().getResourceDepot());
                //reset flag now that it's been queued
            }
    }
    
}


//adds to now queue; other function replaces it
void BuildQueue::addEntryNow(int count, BWAPI::UnitType type){

    bool typeExists = false;
    
    //if type already in queue add to it
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].type){
            typeExists = true; 
            next[i].countWantedNow += count;
            //next[i].countBuiltNow = 0;            
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


//handing queue entry for upgrades
void BuildQueue::addEntryTotal(int count, BWAPI::UpgradeType type, int upgradeLevel){
    
    //if we already have it do nothing
    if(BWAPI::Broodwar->self()->getUpgradeLevel(type) >= upgradeLevel) return;

    for (int i = 0; i < next.size(); ++i){
        if (type == next[i].upType){
            next[i].upgradeLevel = upgradeLevel;
            return;
        }     
    }

    //else create it
    next.push_back(QueueEntry());
    next.back().upType = type;
    next.back().isUpgrade = true;
    next.back().upgradeLevel = upgradeLevel;

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

    //else add it
    if(!typeExists){
        next.push_back(QueueEntry());
        next.back().type = type;
        next.back().countWantedTotal = count;
        next.back().countBuiltTotal = Tools::CountUnitsOfType(type, BWAPI::Broodwar->self()->getUnits() ); //init
    }    
}

void BuildQueue::incrementEntryTotal(int count, BWAPI::UnitType type){

   bool typeExists = false;
    
    //if type already in queue AUGMENT count wanted and reset built
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].type){
            typeExists = true; 
            next[i].countWantedTotal = next[i].countWantedTotal + count;
            next[i].countBuiltTotal=  Tools::CountUnitsOfType(type, BWAPI::Broodwar->self()->getUnits() );           
        }     
    }    

    //else add it
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
        //now subtract any building about to be built by a worker
        for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
            if (unit->getBuildType() == next[index].type){
                totalBuild--;
            }
    }
        if (totalBuild > 0) {
            next[index].buildQty = totalBuild;
            return totalBuild;
        }
    }

    //if difference is 0 or less
    next[index].buildQty = 0;
    return 0;
}

void BuildQueue::replaceEntryNow(int count, BWAPI::UnitType type){

    bool typeExists = false;
    
    //if type already in queue add to it
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

void BuildQueue::rmEntry(BWAPI::UpgradeType type, int level){

    bool typeExists = false;
    int index = -1;
    for (int i = 0; i < next.size() && !typeExists; ++i){
        if (type == next[i].upType && next[i].upgradeLevel == level) typeExists = true;
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

BWAPI::TilePosition BuildQueue::determineLocation(BWAPI::UnitType type){

    //gs should tell us which base to build this building at
    if(type == BWAPI::BroodwarPtr->self()->getRace().getResourceDepot()){
        if(gs->hatcheryMain) return BWAPI::Broodwar->self()->getStartLocation();
        return gs->mapPtr->findNextExpansion(gs)->Location();

    //this will return first owned base that has less refineries than geysers
    }else if (type == BWAPI::BroodwarPtr->self()->getRace().getRefinery()){
        for(auto &baseEcon : gs->ownedBases){
            if(baseEcon.second.assimilatorCount < baseEcon.second.base->Geysers().size()){
                return baseEcon.second.base->Location();
            }
        }
    }
    
    return BWAPI::Broodwar->self()->getStartLocation();

}

//won't work for lurkers, should for all else; should check if we need prereqs at all before calling this
BWAPI::UnitType BuildQueue::queueNextPrereq(BWAPI::UnitType type){

    if(type == BWAPI::UnitTypes::Protoss_Probe || type == BWAPI::UnitTypes::Protoss_Nexus || 
        type == BWAPI::UnitTypes::Protoss_Pylon){
        return BWAPI::UnitTypes::None;
    }

    const std::map<BWAPI::UnitType, int> reqs = type.requiredUnits();

    for (auto const &p : reqs){

        BWAPI::UnitType reqType =  p.first;
        bool aboutToBuild = false;

        //check if on the way to build and if so move on
        for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
            if (unit->getBuildType() == reqType){
                aboutToBuild = true;
                break;
            } 
        }
        if (aboutToBuild) continue;

        //check if already added to queue and if so move on
        bool added = false;
        for (auto& entry : next){
            if(entry.type == reqType){
                added = true;
                break;
            }
        }
        if (added) continue;

        

        //add one to queue if not built yet and not on the way to build 
        if(!Tools::CountUnitsOfType(reqType, BWAPI::Broodwar->self()->getUnits())){
            addEntryNow(1, reqType);
            return reqType; //only building the first one
        }
    } 

    //if we didn't find any reqs
    return BWAPI::UnitTypes::None;
}



// Attempt to construct a building of a given type AT A GIVEN TILE LOCATION
bool BuildQueue::BuildBuilding(BWAPI::UnitType type, BWAPI::Unit *foundBuilder, BWAPI::TilePosition desiredPos){

    // Get the type of unit that is required to build the desired building
    BWAPI::UnitType builderType = type.whatBuilds().first;
    BWAPI::Unit builder = Tools::GetBuilder(builderType);
    if (!builder) { return false; }
    *foundBuilder = builder;

    // Desired location is argument now
    //if we are morphing do that now
    if(builderType.isBuilding()){
        return builder->morph(type);
    }

    // Ask BWAPI for a building location near the desired position for the type
    int maxBuildRange = 64;
    bool buildingOnCreep = type.requiresCreep();
    BWAPI::TilePosition buildPos = BWAPI::Broodwar->getBuildLocation(type, desiredPos, maxBuildRange, buildingOnCreep);
    
    return builder->build(type, buildPos);
}

BuildResult BuildQueue::TrainUnit(BWAPI::UnitType type){

    //GETTING SOURCE BUILDING/UNIT IGNORING ARCHONS for now
    BWAPI::UnitType trainerType = type.whatBuilds().first;
    BWAPI::Unitset trainerSet;

    // For each unit that we own
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()){
        // if the unit is of the correct type and it actually has been constructed, add it to set
        if (unit->getType() == trainerType && unit->isCompleted() && !unit->isMorphing()){
            trainerSet.insert(unit);
        }
    }

    if(trainerSet.size() == 0) return NO_TRAINER;

    for (auto& trainer : trainerSet){
        if (!trainer->isTraining()){
            if (trainer->train(type)){
                return QUEUED; //success
            }else return FAILED; //failed for some reason
        }
    }

    return QUEUE_FULL; //presumably all trainers full
    
}


//NOW IBO PARSING

void InitialBuildOrder::nextStep(int dblSupplyCount, int* targetCount){

    int supplyCount = dblSupplyCount; //ibo was doubled already whoops
        
    //assume constant droning unless we exceed count per gs.. should not happen on ibo
    bool droning = (bq->gs->workerCount < bq->gs->workerMax);
    BWAPI::UnitType worker = BWAPI::Broodwar->self()->getRace().getWorker();
    BWAPI::UnitType pylon = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    //always have queued one drone unless we cut drones
    if(droning){ 
        bool droneQueued = false;
        for (auto& entry : bq->next){
            if(entry.type == worker){
                droneQueued = true;
                break;
            }
        }
        if(!droneQueued) bq->addEntryNow(1, worker);
    }

    //also build prescribed building or unit but only one per step   
    for (int i = 0; i < ibo_supplyCount.size(); ++i){
        if (supplyCount >= ibo_supplyCount[i]){
            curStep = i;
            if (lastStep < curStep){ //if new step 
                bq->addEntryNow(1, ibo_unitType[i]); //queue one up
                if ((curStep + 1) == ibo_supplyCount.size()) isFinished = true;
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
int InitialBuildOrder::load_ibo(std::string path){

    std::ifstream infile (path);
    std::string instring;
    char inchar;
    int inint;
    bool isCount = true;
    int i = 0;
    
    if(infile.is_open()){
        //first loop parses unit type and supply count until *
        while (infile.good()){
        //parsing unittype and supply count; at # we switch to instructiobs
            if (inchar == '*') break; //on * skip to instructions
            if(inchar == '#'){ //on # call it a comment and skip that line
                getline(infile, instring);
                instring.clear();
            } 
            inchar = infile.get();
            if( inchar != ' ' && inchar != '\n'){
                instring += inchar;
            }else{ //on newline or space parse int
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

        //second loop to store instructions after int based ibo
        //instructions are fed to gs
        //* makes instruction start
        //# makes comment; $makes timestamp; other int is supplyCount; other alpha is instruction
        while (infile.good()){
            //assuming we have one instruction at least
            if(bq->gs->instructions.size() == 0) bq->gs->instructions.push_back(InstructionEntry());
            //advance line on hashmark or *; this should also allow addl comments
            if(inchar == '*' || inchar == '#'){
                getline(infile, instring);
                instring.clear();
            } 
            inchar = infile.get();
            if( inchar != ' ' && inchar != '\n'){
                instring += inchar; //add until space or newline
            
            //now handling strings
            //if starts with $ store a timestamp
            }else if(*(instring.begin()) == '$' ){
                instring.erase(instring.begin());
                inint = std::stoi(instring, NULL);
                (*bq->gs->instructions.rbegin()).time = inint;
                instring.clear(); 
            }
                    
            //if starts with letter store an instruction and advance
            else if (std::isalpha(*(instring.begin()))) {
                (*bq->gs->instructions.rbegin()).inst = parseInstruction(instring);
                instring.clear(); 

                //advance
                bq->gs->instructions.push_back(InstructionEntry());
                
            //else should be a supply count number
            }else{
                inint = std::stoi(instring, NULL);
                (*bq->gs->instructions.rbegin()).supply = inint;
                instring.clear(); 
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

    for(auto &entry : bq->gs->instructions){
        entry.supply = (entry.supply * 2);
    }
    
    return i;
}


Instruction InitialBuildOrder::parseInstruction(const std::string& instr){

    if(instr == "EXPAND_NATURAL") return EXPAND_NATURAL;
    if(instr == "HATCHERY_MAIN") return HATCHERY_MAIN;
    if(instr == "LING_SPEED") return LING_SPEED;

    return NO_INSTRUCTION;

}


//NOW TRACKING (AS RELATES TO BUILDERS ONLY)
void Tracker::onFrame(){
    
    //builders
    std::vector<int> completedBuilders;

    for(auto& entry: BuilderList){
        if(entry.second.unit == nullptr){
            continue; //error checking
        } 
        CommandResult result = didBuilderSucceed(entry.first, entry.second);
        if (result == FAIL_AND_RETRY) bq->addEntryNow(1, entry.second.buildType);
        if (result != ONGOING){ //we either built or failed; either way release the funds 
            completedBuilders.push_back(entry.first);
            bq->gasCommited -= entry.second.buildType.gasPrice();
            bq->minCommited -= entry.second.buildType.mineralPrice();
            bq->supplyCommited -= entry.second.buildType.supplyRequired(); //this maybe makes no sense to track?
        }
    }

    //delete former builders
    for(auto& key: completedBuilders){
        BuilderList.erase(key);
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

    //if we are zerg this is simpler?
    if(BWAPI::Broodwar->self()->getRace() == BWAPI::Races::Zerg){
        if(found.unit->getType() == found.buildType){
            return SUCCESS;
        }else{
            if (found.unit->getBuildType() != found.buildType){
                return FAIL_AND_RETRY;
            }else{
                return ONGOING;
            }
        }

    }else{
        //for non zergs
        //If buildtype does not matches unit is no longer on the way
        if(found.unit->getBuildType() != found.buildType){
            //now count to see if he built it
            int newCount = Tools::CountUnitsOfType(found.buildType,BWAPI::Broodwar->self()->getUnits());
            if (newCount <= found.initBuildCount){ 
                return FAIL_AND_RETRY;
            }else{
                return SUCCESS;}
        }else{
            return ONGOING;
        }
    }

}

void BuildQueue::clearEmptyEntries(){

    for (auto& entry : next){
        if(entry.buildQty < 1)
        rmEntry(entry.type);
    }

}

