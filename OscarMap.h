//Tools for understanding the map specific to this bot
#include "BWAPI.h"
#include "BWEM/bwem.h"
#include <vector>

class OscarMap{

public:
    BWEM::Base* main;
    BWEM::Base* natural;
    BWEM::Base* nextExp;

    BWEM::Base* enemyMain;
    BWEM::Base* enemyNatural;

    std::vector<const BWEM::Base*> mainBases;
    std::vector<const BWEM::Base*> natBases;
    std::vector<const BWEM::Base*> otherBases;

    OscarMap(){};

    void onStart();
    void onFrame();


private:    
    void findBases();

};
