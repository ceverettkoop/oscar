#include <BWAPI.h>
#include "GameState.h"


class Decider{

public:  
    Decider(){};
    
    void onStart();
    void onFrame();

private:
    bool setScouting(GameState* gs);

};