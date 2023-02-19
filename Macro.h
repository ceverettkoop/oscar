#include <BWAPI.h>

class MacroManager;

class MacroManager{

private:
    BWAPI::TilePosition enemyLocation = BWAPI::TilePositions::Unknown;
	BWAPI::Unit scout = nullptr;
	bool isScouting = false;

    void scouting(BWAPI::Unit scout, BWAPI::TilePosition enemyLocation);

public:
    void onFrame();

};