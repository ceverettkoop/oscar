#include "BWAPI.h"
#include "BWEM/bwem.h"
#include "OscarMap.h"
#include <vector>
#include <cfloat>


void OscarMap::onStart(){

    findBases();

}

void OscarMap::onFrame(){
    
}

//code here largely taken from BWEB
//using BWEM distance algo instead of BWEB one
//https://github.com/Cmccrave/BWEB/
void OscarMap::findBases(){

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

    //assign our natural
    size_t min = INT16_MAX;
    const BWEM::Base * baseBest = nullptr;

    for (auto &base : natBases){

        const BWEM::CPPath & Path = BWEM::Map::Instance().GetPath(main->Center(), base->Center() );
        const auto dist = Path.size();
        if(dist < min){
            min = dist;
            baseBest = base;
        } 

    }

    natural = baseBest;

}