#pragma once

#include "MapTools.h"
#include <BWAPI.h>
#include <vector>

enum BuildResult{
    NODATA,
	NOT_ENOUGH_RESOURCES,
    QUEUED,
    QUEUE_FULL,
	NO_TRAINER,
    FAILED
};

class StarterBot;


class InitialBuildOrder{
	//just stores what to build at drone count x
	
private:
	std::vector<int> ibo_supplyCount; //requisite count to build unit or struct at i
	std::vector<BWAPI::UnitType> ibo_unitType; //type of unit to build
	

public:
	
	int stepCount = 0;
	bool isFinished = false;
	int load_ibo(char* path);
	StarterBot *const bot;
	
	//constructor w reference to parent bot
	InitialBuildOrder(StarterBot* const inbot):bot(inbot){}
	StarterBot* getBot(){return bot;}

	//functions
	BWAPI::UnitType nextStep(int supplyCount, int* targetCount);
	int DesiredCountAlreadyBuilt(BWAPI::UnitType type);

};

class BuildQueue{

public:

	BWAPI::UnitType next; //next thing to build, currently one at a time
	bool onIbo = true;
	StarterBot* const bot;
	BuildResult lastResult;
	BWAPI::UnitType lastAttempt;

	//constructor w reference to parent bot
	BuildQueue(StarterBot* const inbot):bot(inbot){}
	StarterBot* getBot(){return bot;}
	
	void updateQueue();



};


class StarterBot : public BWAPI::AIModule
{
    MapTools m_mapTools;


public:

	InitialBuildOrder ibo = InitialBuildOrder(this);
	BuildQueue bq = BuildQueue(this);

    StarterBot();

    //my functions to have bot do stuff (bot thinking elsewhere)
	void buildNext();
	
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


};

