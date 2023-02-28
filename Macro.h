#include <BWAPI.h>


class MacroManager{

private:
	BWAPI::Unit scout = nullptr;
	bool isScouting = false;

    void scouting(BWAPI::Unit scout, BWEM::Base* enemyMain);

public:
    void onFrame(GameState* gs);

};

