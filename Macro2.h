#pragma once

#include <BWAPI.h>
#include "GameState.h"


class MacroManager{

private:
    //scouting
    void scouting();

    //economy
    void assignWorkers();
    int countMinersInBase(const BWEM::Base *base);
    int countGasCollectorsInBase(const BWEM::Base *base);

    //building
    
    
    

public:
    void onStart();
    void onFrame();
    GameState* gs;

};

