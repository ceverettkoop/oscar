#pragma once

//Tools for understanding the map specific to this bot
#include "BWAPI.h"
#include "BWEM/bwem.h"
#include <vector>

class GameState;

class OscarMap{

public:
    const BWEM::Base* myMain = nullptr;
    const BWEM::Base* myNatural = nullptr;

    const BWEM::Base* enemyMain = nullptr;
    const BWEM::Base* enemyNatural = nullptr;

    std::vector<const BWEM::Base*> allBases;
    std::vector<const BWEM::Base*> mainBases;
    std::vector<const BWEM::Base*> natBases;
    std::vector<const BWEM::Base*> otherBases;

    OscarMap(){};

    void onStart();
    void onFrame();
    const BWEM::Base * findNextExpansion(GameState *gs);

    void OscarMap::assignNatural();


private:    
    void findBases();


};
