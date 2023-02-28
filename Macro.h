#include <BWAPI.h>


class MacroManager{

private:
	BWAPI::Unit scout = nullptr;
	bool isScouting = false;

    void scouting(BWAPI::Unit scout, GameState* gs);

public:
    void onFrame(GameState* gs);

};

