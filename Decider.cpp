#include "Decider.h"
#include "OscarMap.h"
#include "GameState.h"


void Decider::onFrame(GameState* gs){

    //decide if we are scouting
    setScouting(gs);

}


//logic to scout at X supply until we find the enemy
//will be made more complicated
void Decider::setScouting(GameState* gs){

    const int scoutStart = 16;

    if(gs->mapPtr->enemyMain == nullptr){
            if(BWAPI::Broodwar->self()->supplyUsed() > scoutStart){
                gs->isScouting = true;
            }
    }else(gs->isScouting = false); //this is if we found the enemy

}
