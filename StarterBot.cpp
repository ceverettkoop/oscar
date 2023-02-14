#ifndef WIN32 //if compiling not on windows
#include <unistd.h>
#else
#include <direct.h>
#endif

#include <cstring>
#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"


StarterBot::StarterBot()
{
    
}

// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();

    //load ibo (TODO make path relative; load multiple ibos etc)
    char path[512];
    #ifndef WIN32
    getcwd(path, 400);
    #else
    _getcwd(path, 200);
    #endif
    strncat(path,"/bwapi-data/read/2gatewaypvz", 111);
    fprintf(stderr,"IBO path is %s\n", path);

    ibo.load_ibo(path);
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    //built next unit/building or wait for money to accrue
    buildNext();

    // Train more workers so we can gather more income
    //trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    //buildAdditionalSupply();

    // Draw unit health bars, which brood war unfortunately does not do
    //Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
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

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 20;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation(){

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

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
	
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }
}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
	
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}

//Iterate through entries in building queue and build what we are supposed to build
void StarterBot::buildNext(){

    bq.updateQueue();

    for (int i = 0; i < bq.next.size(); i++){
        
        BWAPI::UnitType nextUnit = bq.next[i].type;

        //first check if prereqs met; if not add them to queue and skip to the end
        if (!BWAPI::Broodwar->self()->isUnitAvailable(nextUnit)){
            bq.queueNextPrereq(nextUnit);
        }else{
        
            bool canAfford = ( nextUnit.mineralPrice() <= BWAPI::Broodwar->self()->minerals() && 
                nextUnit.gasPrice() <= BWAPI::Broodwar->self()->minerals() &&
                nextUnit.supplyRequired() <= (BWAPI::Broodwar->self()->supplyTotal() - BWAPI::Broodwar->self()->supplyUsed()));

            int countToBuild = 0;

            if(canAfford){
                int countToBuild = bq.updateQty(i); //check how many to build

                if( countToBuild){ //check if counts for this entry make sense or we're on ibo 
                    if(nextUnit.isBuilding()){
                        for (int n = 0; n < countToBuild; ++n){
                            if(Tools::BuildBuilding(nextUnit)) bq.next[i].countBuiltNow++;
                        }
                        
                    }else{
                        for (int n = 0; n < countToBuild; ++n){
                            if(Tools::TrainUnit(nextUnit) == QUEUED) bq.next[i].countBuiltNow++;
                        }
                    }
                }
            }
        }
    }
    
    return;
    
}