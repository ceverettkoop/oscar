#pragma once

#include "MapTools.h"

#include <BWAPI.h>
#include <vector>

class StarterBot : public BWAPI::AIModule
{
    MapTools m_mapTools;

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();

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

	//build order
	


};

class InitialBuildOrder{
	//just stores what to build at drone count x
	std::vector<int> ibo_droneCount; //requisite count to build unit or struct at i
	std::vector<bool> ibo_isUnit; //is unit or building

public:
	bool is_ibo_finished(int droneCount);
	int unitToBuild(int droneCount);
	
	void load_ibo(std::string path);
};