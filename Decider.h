#pragma once
#include <BWAPI.h>

class GameState;

class Decider{

public:  
    Decider(){};
    
    void onStart(GameState* gs);
    void onFrame(GameState* gs);

private:
    void setScouting(GameState* gs);

};