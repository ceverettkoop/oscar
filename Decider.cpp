#include "Decider.h"
#include "OscarMap.h"


void Decider::onFrame(GameState* gs){

    //decide if we are scouting
    setScouting(gs);


}


//logic to scout at X supply until we find the enemy
//will be made more complicated
bool Decider::setScouting(GameState* gs){

    const int scoutStart = 16;

    if(gs->mapPtr->enemyMain == nullptr){
        if(gs->scouting){
            if(BWAPI::Broodwar->self()->supplyUsed() > scoutStart){
                gs->scouting = true;
            }
        }
    }else(gs->scouting = false); //this is if we found the enemy

}
