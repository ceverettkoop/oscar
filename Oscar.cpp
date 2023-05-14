#include <cstring>
#include "Oscar.h"
#include "Tools.h"


// Called when the bot starts!
void Oscar::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    //Initalize BWEM, will replace above if possible
    BWEM::Map::Instance().Initialize(BWAPI::BroodwarPtr);
    BWEM::Map::Instance().EnableAutomaticPathAnalysis();
    BWEM::Map::Instance().FindBasesForStartingLocations();

    //start internal map manager
    gs.onStart();


}

// Called on each frame of the game
void Oscar::onFrame(){

    //Update Oscars map tools
    gs.MapOnFrame();
    gs.UpdateUnits();

    

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}


// Draw some relevent information to the screen to help us debug the bot
void Oscar::drawDebugInformation(){

    /*//draw build queue
    char format[1024] = "Current build priority:\n";
    char qty[16] = "";
    for(int i = 0; i < bq.next.size(); i++){
        snprintf(qty, 15, " x %d\n", bq.next[i].buildQty);
        bq.next[i].isUpgrade ? strncat(format, bq.next[i].upType.getName().c_str(), 63) :
            strncat(format, bq.next[i].type.getName().c_str(), 63);
        strcat(format, qty);
        if (strlen(format) > 943) break; //overflow protection
    }
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Minerals:  %d\nGas: %d\nSupply %d/%d\n%s\nApprox seconds: %d\n"
    , BWAPI::Broodwar->self()->minerals(), BWAPI::Broodwar->self()->gas(), ( BWAPI::Broodwar->self()->supplyUsed() / 2), 
    (BWAPI::Broodwar->self()->supplyTotal() / 2), format, (BWAPI::Broodwar->getFrameCount() / 24) );
    */

    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();

}


// Called whenever the game ends and tells you if you won or not
void Oscar::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}

// Called whenever a unit is destroyed, with a pointer to the unit
void Oscar::onUnitDestroy(BWAPI::Unit unit){


	
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void Oscar::onUnitMorph(BWAPI::Unit unit){


	
}

// Called whenever a text is sent to the game by a user
void Oscar::onSendText(std::string text) 
{ 


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