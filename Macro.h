#pragma once

#include <BWAPI.h>
#include "GameState.h"


class MacroManager{

private:
	BWAPI::Unit scout = nullptr;
	bool isScouting = false;
    bool natExplored = false;

    void scouting();
    void assignWorkers();
    int countMinersInBase(const BWEM::Base *base);
    int countGasCollectorsInBase(const BWEM::Base *base);

public:
    void onStart();
    void onFrame();
    GameState* gs;

};

