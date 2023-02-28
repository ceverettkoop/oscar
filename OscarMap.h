//Tools for understanding the map specific to this bot
#include "BWAPI.h"
#include "BWEM/bwem.h"
#include <vector>

class OscarMap{

public:
    const BWEM::Base* main = nullptr;
    const BWEM::Base* natural = nullptr;
    const BWEM::Base* nextExp = nullptr;

    const BWEM::Base* enemyMain = nullptr;
    const BWEM::Base* enemyNatural = nullptr;

    std::vector<const BWEM::Base*> mainBases;
    std::vector<const BWEM::Base*> natBases;
    std::vector<const BWEM::Base*> otherBases;

    OscarMap(){};

    void onStart();
    void onFrame();


private:    
    void findBases();

};
