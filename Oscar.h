//Oscar BW bot
//meant to compile on Linux/OpenBW or with Visual Studio on Win32
//main structure based on Starterbot from Dave Churchill

#pragma once

#include "BuildOrder.h"
#include "Macro.h"
#include "MapTools.h"
#include <BWAPI.h>
#include <vector>
#include <BWEM/bwem.h>


class Oscar;


class Oscar : public BWAPI::AIModule
{
    MapTools m_mapTools;

public:

	BuildQueue bq = BuildQueue();
	MacroManager macro = MacroManager();

    Oscar();

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

