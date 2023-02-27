#include "Decider.h"


//logic to scout at X supply until we find the enemy
//will be made more complicated
bool Decider::setScouting(GameState* gs){

    const int scoutStart = 16;

    if(enemyLocation == BWAPI::TilePositions::Unknown){
        if(gs->scouting){
            if(BWAPI::Broodwar->self()->supplyUsed() > scoutStart){
                isScouting = true;
            }
        }
    }else(gs->scouting = false); //this is if we found the enemy

}
