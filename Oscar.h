//Oscar BW bot
//meant to compile on Linux/OpenBW or with Visual Studio on Win32
//main structure based on Starterbot from Dave Churchill

#pragma once

#include "GameState.h"
#include "Decider.h"
#include "Map/OscarMap.h"
#include "BuildOrder.h"
#include "Macro.h"

#include "Map/MapTools.h"
#include <BWAPI.h>
#include <vector>
#include <BWEM/bwem.h>


class Oscar;


class Oscar : public BWAPI::AIModule
{
    MapTools m_mapTools;

public:

	Decider decider;
	GameState gs;
	BuildQueue bq = BuildQueue();
	MacroManager macro = MacroManager();
	OscarMap map = OscarMap();

    Oscar(){};

	//general bot actions
    void sendIdleWorkersToMinerals();
    void drawDebugInformation();
	void collectGas(int countPerGeyser);

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);


};

