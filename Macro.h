#include "StarterBot.h"
#include <BWAPI.h>

class Army{
    
public:
    int size; //# of units
    std::vector<BWAPI::Unit*> members; //pointers to units

    //create new army by indicating first member
    Army(BWAPI::Unit* firstUnit); 
    void addUnit(BWAPI::Unit* inUnit);
    //void removeUnit(Unit* inUnit); implement later but not needed now
    void updateArmy();
    void commandAll(BWAPI::UnitCommandType command);

};
