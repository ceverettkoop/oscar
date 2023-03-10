#include "GameState.h"

void GameState::passInstruction(const std::string instring){

    if(instring == "SCOUT") instruction = SCOUT;
    else if (instring == "EXPAND") instruction = EXPAND;
    else instruction = NO_INSTRUCTION;
}
