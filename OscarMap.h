#pragma once

//Tools for understanding the map specific to this bot
#include "BWAPI.h"
#include "BWEM/bwem.h"
#include <vector>

struct BaseEconomy{

    const BWEM::Base * base;
    int minCount = 0;
    int assimilatorCount = 0;
    int minerCount = 0;
    int gasMinerCount = 0;
    Owner owner = NEUTRAL;
    
    //int basePriority = 3; //program should set to 0 if main and 1 if natural

};

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

    std::map<int, BaseEconomy> knownBases;

    OscarMap(){};

    void onStart();
    void onFrame();
    const BWEM::Base * findNextExpansion(GameState *gs);

    void assignNatural();


private:    
    void findBases();


};
