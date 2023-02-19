#ifndef WIN32 //if compiling not on windows
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <cstring>
#include "Oscar.h"
#include "Tools.h"


Oscar::Oscar()
{
    
}

// Called when the bot starts!
void Oscar::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();

    //Initalize BWEM, will replace above if possible
    BWEM::Map::Instance().Initialize(BWAPI::BroodwarPtr);
    BWEM::Map::Instance().EnableAutomaticPathAnalysis();

    //load ibo (TODO make path relative; load multiple ibos etc)
    char path[512];
    #ifndef WIN32
    getcwd(path, 400);
    #else
    _getcwd(path, 200);
    #endif
    strncat(path,"/bwapi-data/read/2gatewaypvz", 111);
    fprintf(stderr,"IBO path is %s\n", path);

    bq.onStart(path);

    //this will get kicked to macro object
    enemyLocation = BWAPI::TilePositions::Unknown;
}

// Called on each frame of the game
void Oscar::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    //divert workers to gas if we have less than X
    //and if we have an assimilator
    collectGas(3);

    //All building of units or buildings
    bq.onFrame();

    //scout - this will get moved
    //TODO MOVE THIS ELSEWHERE; for now decision if we are scouting is here
    if(enemyLocation == BWAPI::TilePositions::Unknown){
        if(isScouting){
            if(BWAPI::Broodwar->self()->supplyUsed() > 16){
                isScouting = true;
            }
        }
    }else(isScouting = false); //this is if we found the enemy

    if (isScouting) scouting(scout, enemyLocation);


    // Draw unit health bars, which brood war unfortunately does not do
    //Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

// Send our idle workers to mine minerals so they don't just stand there
void Oscar::sendIdleWorkersToMinerals()
{

    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

void Oscar::collectGas(int countPerGeyser){

    BWAPI::Unitset refineries = Tools::GetUnitSetofType(BWAPI::Broodwar->self()->getRace().getRefinery());
    if(refineries.size() == 0) return;

    //how many workers are collecting gas
    //this will need to get more complex for multibase
    //sending one per frame TODO stagger them
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    int foundGatherers = 0;
    for (auto& unit : myUnits){
        if (unit->getType().isWorker() && unit->isGatheringGas()) foundGatherers++;
    }

    if(foundGatherers < countPerGeyser){
        for (auto& unit : myUnits){
            if (unit->getType().isWorker() && unit->isGatheringMinerals()){
                BWAPI::Unit closestGas = Tools::GetClosestUnitTo(unit, refineries);
                unit->gather(closestGas);
                break;
            }
        }
    }

}


// Draw some relevent information to the screen to help us debug the bot
void Oscar::drawDebugInformation(){

    char format[1024] = "Current build priority:\n";
    char qty[16] = "";
    for(int i = 0; i < bq.next.size(); i++){
        snprintf(qty, 15, " x %d\n", bq.next[i].buildQty);
        strncat(format, bq.next[i].type.getName().c_str(), 63); 
        strcat(format, qty);
        if (strlen(format) > 943) break; //overflow protection
    }
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Minerals:  %d\nGas: %d\nSupply %d/%d\n%s\n"
    , BWAPI::Broodwar->self()->minerals(), BWAPI::Broodwar->self()->gas(), ( BWAPI::Broodwar->self()->supplyUsed() / 2), 
    (BWAPI::Broodwar->self()->supplyTotal() / 2), format );

    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();

}

//this needs to get moved to macro
void Oscar::scouting(BWAPI::Unit scout, BWAPI::TilePosition enemyLocation){


    //assign scout if needed
    if(scout == nullptr || !scout->exists()){ //if scout not assigned or dead
            scout = Tools::GetUnitOfType(BWAPI::Broodwar->self()->getRace().getWorker());
    }else scout = nullptr; //if not scouting release any unit from being called a scout


    auto& startLocations = BWAPI::Broodwar->getStartLocations();

    for(BWAPI::TilePosition tpos : startLocations){
        if(BWAPI::Broodwar->isExplored(tpos)) continue;

        BWAPI::Position pos(tpos);
        scout->move(pos);

        if(BWAPI::Broodwar->isExplored(tpos)){
            BWAPI::Unitset baseUnits = BWAPI::Broodwar->getUnitsOnTile(tpos);
            for(auto& unit : baseUnits){
                if(BWAPI::Broodwar->self()->isEnemy(unit->getPlayer())) enemyLocation = tpos;
            }    
        }
        
        break;
    }

}

// Called whenever the game ends and tells you if you won or not
void Oscar::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void Oscar::onUnitDestroy(BWAPI::Unit unit)
{
	
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void Oscar::onUnitMorph(BWAPI::Unit unit)
{
	
}

// Called whenever a text is sent to the game by a user
void Oscar::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void Oscar::onUnitCreate(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit finished construction, with a pointer to the unit
void Oscar::onUnitComplete(BWAPI::Unit unit)
{
	
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void Oscar::onUnitShow(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void Oscar::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void Oscar::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}