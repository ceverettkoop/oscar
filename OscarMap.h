//Tools for understanding the map specific to this bot
#include "BWAPI.h"
#include "BWEM/bwem.h"
#include <vector>

class OscarMap{

public:
    BWEM::Base* main = nullptr;
    BWEM::Base* natural = nullptr;
    BWEM::Base* nextExp = nullptr;

    BWEM::Base* enemyMain = nullptr;
    BWEM::Base* enemyNatural = nullptr;

    std::vector<const BWEM::Base*> mainBases;
    std::vector<const BWEM::Base*> natBases;
    std::vector<const BWEM::Base*> otherBases;

    OscarMap(){};

    void onStart();
    void onFrame();


private:    
    void findBases();

};
