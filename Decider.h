#include <BWAPI.h>
#include "GameState.h"


class Decider{

public:  
    Decider(){};
    
    void onStart(GameState* gs);
    void onFrame(GameState* gs);

private:
    bool setScouting(GameState* gs);

};