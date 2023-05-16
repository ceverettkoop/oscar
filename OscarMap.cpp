#include "BWAPI.h"
#include "BWEM/bwem.h"
#include "OscarMap.h"
#include <vector>
#include <cfloat>
#include "GameState.h"


void OscarMap::onStart(){

    findBases();

}

void OscarMap::onFrame(){
    
}

//code here largely taken from BWEB
//using BWEM distance algo instead of BWEB one
//https://github.com/Cmccrave/BWEB/
void OscarMap::findBases(){


    //create set of ALL Bases
    for (auto &area : BWEM::Map::Instance().Areas()) {
        
        for (auto &base : area.Bases()) {
                allBases.push_back(&base);
        }
    }

    // Find all main bases
    for (auto &area : BWEM::Map::Instance().Areas()) {
        for (auto &base : area.Bases()) {
            if (base.Starting())
                mainBases.push_back(&base);
        }
    }

    // Find all natural bases
    for (auto &main : mainBases) {

        const BWEM::Base * baseBest = nullptr;
        auto distBest = DBL_MAX;

        for (auto &area : BWEM::Map::Instance().Areas()) {
            for (auto &base : area.Bases()) {

                // Must have gas, be accesible and at least 5 mineral patches
                if (base.Starting()
                    || base.Geysers().empty()
                    || base.GetArea()->AccessibleNeighbours().empty()
                    || base.Minerals().size() < 5)
                    continue;
                
                const BWEM::CPPath & Path = BWEM::Map::Instance().GetPath(main->Center(), base.Center() );
                const auto dist = Path.size();

                if (dist < distBest && dist < 1600.0) {
                    distBest = dist;
                    baseBest = &base;
                }
            }
        }

        // Store any natural we found
        if (baseBest)
            natBases.push_back(baseBest);

        // Refuse to not have a natural for each main, try again but less strict
        else {
            for (auto &area : BWEM::Map::Instance().Areas()) {
                for (auto &base : area.Bases()) {
                    if (base.Starting())
                        continue;

                const BWEM::CPPath & Path = BWEM::Map::Instance().GetPath(main->Center(), base.Center() );
                const auto dist = Path.size();

                    if (dist < distBest) {
                        distBest = dist;
                        baseBest = &base;
                    }
                }
            }

            // Store any natural we found
            if (baseBest)
                natBases.push_back(baseBest);
        }
    }

}

const BWEM::Base * OscarMap::findNextExpansion(const GameState& gs){
    
    //take natural if not already occupied
    auto nat = knownBases.find(1);
    if(nat->second.owner != ALLIED){ return nat->second.base;

    //else ogre expansion logic
    bool occupied = false;
    const BWEM::Base* baseBest = nullptr;
    int distBest = 0;

    for(auto &base : allBases){

        auto knownBase = knownBases.find(getBaseKey(base));
        if( knownBase->second.owner != NEUTRAL ) continue;

        //if unoccupied check distance
        const BWEM::CPPath & Path = BWEM::Map::Instance().GetPath(myMain->Center(), base->Center() );
        const auto dist = Path.size();

        if (dist < distBest) {
            distBest = dist;
            baseBest = base;
        }
    }

        return baseBest;
    
    }
    


}

void OscarMap::assignNatural(){

    size_t min = INT16_MAX;
    const BWEM::Base * baseBest = nullptr;

    for (auto &base : natBases){

        const BWEM::CPPath & Path = BWEM::Map::Instance().GetPath(myMain->Center(), base->Center() );
        const auto dist = Path.size();
        if(dist < min){
            min = dist;
            baseBest = base;
        } 
    }

    myNatural = baseBest;

}

int OscarMap::getBaseKey(const BWEM::Base * base){

    //unique key per base
    //0 for main; 1 for nat; other bases pseudo unique value based on distance from main
    if (base == myMain){
        return 0;
    }else if (base == myNatural){
        return 1;
    }else{ //generate key based on distance from main + distance from nat (SHOULD be unique)
        return ( 100 + base->Center().getApproxDistance(myMain->Center()) +
            base->Center().getApproxDistance(myNatural->Center()) );
    }
}