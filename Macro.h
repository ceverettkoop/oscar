#include <BWAPI.h>
#include "GameState.h"


class MacroManager{

private:
	BWAPI::Unit scout = nullptr;
	bool isScouting = false;

    void scouting();
    void assignWorkers();

public:
    void onStart();
    void onFrame();
    GameState* gs;

};

