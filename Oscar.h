//Oscar BW bot
//meant to compile on Linux/OpenBW or with Visual Studio on Win32
//based on Starterbot from Dave Churchill

#pragma once

#include "MapTools.h"
#include <BWAPI.h>
#include <vector>
#include <BWEM/bwem.h>

enum BuildResult{
    NODATA,
	NOT_ENOUGH_RESOURCES,
    QUEUED,
    QUEUE_FULL,
	NO_TRAINER,
    FAILED
};

enum BuildGoal{
	BUILD_DRONES,
	EXPAND,
	BUILD_ZEALOTS,
	BUILD_DRAGOONS,
	BUILD_FORGE
};

enum CommandResult{
	FAIL_AND_RETRY,
	SUCCESS,
	ONGOING
};

class Oscar;


class InitialBuildOrder{
	//just stores what to build at drone count x
	
private:
	std::vector<int> ibo_supplyCount; //requisite count to build unit or struct at i
	std::vector<BWAPI::UnitType> ibo_unitType; //type of unit to build
	
public:
	
	int curStep = -1;
	int lastStep = -1;
	int stepCount = 0;
	bool isFinished = false;
	int load_ibo(char* path);
	Oscar *const bot;
	
	//constructor w reference to parent bot
	InitialBuildOrder(Oscar* const inbot):bot(inbot){}
	Oscar* getBot(){return bot;}

	//functions
	void nextStep(int dblSupplyCount, int* targetCount);
	int DesiredCountAlreadyBuilt(BWAPI::UnitType type);

};


class QueueEntry{

public:
	
	BWAPI::UnitType type = BWAPI::UnitTypes::None;
	int countWantedNow = -1; //-1 means not tracking
	int countBuiltNow = 0; //count built on this iteration, when count wanted equals count built reset
	int countWantedTotal = -1; //-1 means not tracking
	int countBuiltTotal = 0; //retrieve periodically
	int buildQty = 0;

	QueueEntry(){};
	
	QueueEntry(BWAPI::UnitType input){
		type = input;
	}

};


class BuildQueue{


public:

	int baseCount = 1;
	static const int maxSize = 5;
	std::vector<QueueEntry> next; //types of things we are currently trying to build
	bool onIbo = true;
	Oscar* const bot;

	//constructor w reference to parent bot
	BuildQueue(Oscar* const inbot):bot(inbot){
	}
	Oscar* getBot(){return bot;}
	
	void updateQueue();
	int updateQty(int index);
	BuildGoal GetGoal(int supplyUsed, int droneCount, int baseCount);
	void addEntryNow(int count, BWAPI::UnitType type);
	void addEntryTotal(int count, BWAPI::UnitType type);
	void rmEntry(BWAPI::UnitType type);
	void entryToFront(BWAPI::UnitType type); 
	//todo more clever prioritization functions
	//note bool BWAPI::PlayerInterface::isUnitAvailable (UnitType unit) const will tell if can build
	BWAPI::UnitType queueNextPrereq(BWAPI::UnitType type);


};

class GameState{

public:
	bool scouting = false;
	BWAPI::TilePosition enemyLocation = BWAPI::TilePositions::Unknown;
	Oscar* const bot;

	//constructor w reference to parent bot
	GameState(Oscar* const inbot):bot(inbot){
	}
	Oscar* getBot(){return bot;}

};


class Tracker{

private:
	
	struct Builder{
		BWAPI::Unit unit; //specific builder we are tracking
		BWAPI::UnitType buildType; //what the builder is trying to build
		int initBuildCount = 0;
	};

	std::map<int,Builder>BuilderList;

	//track other things?

public:

	int lastKey = -1;
	Oscar* const bot;
	BWAPI::Unit scout = nullptr;

	//constructor w reference to parent bot
	Tracker(Oscar* const inbot):bot(inbot){
	}
	Oscar* getBot(){return bot;}


	void onFrame();
	int trackBuilder(BWAPI::Unit unit, BWAPI::UnitType buildType); //returns index to tracked builder
	CommandResult didBuilderSucceed(int key, Builder found);

};

class Oscar : public BWAPI::AIModule
{
    MapTools m_mapTools;

public:

	InitialBuildOrder ibo = InitialBuildOrder(this);
	BuildQueue bq = BuildQueue(this);
	Tracker track = Tracker(this);
	GameState gs = GameState(this);

    Oscar();

    //my functions to have bot do stuff (bot thinking elsewhere)
	void buildNext();
	
	//general bot actions
    void sendIdleWorkersToMinerals();
    void drawDebugInformation();
	void collectGas(int countPerGeyser);
	void scout();

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

