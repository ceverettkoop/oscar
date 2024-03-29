#pragma once

#include <BWAPI.h>
#include "GameState.h"

enum CommandResult{
	FAIL_AND_RETRY,
	SUCCESS,
	ONGOING
};

enum BuildResult{
    NODATA,
	NOT_ENOUGH_RESOURCES,
    QUEUED,
    QUEUE_FULL,
	NO_TRAINER,
    FAILED
};

class BuildQueue;

class Tracker{

private:
	
	struct Builder{
		BWAPI::Unit unit; //specific builder we are tracking
		BWAPI::UnitType buildType; //what the builder is trying to build
		int initBuildCount = 0;
	};

public:

	int lastKey = -1;
	BuildQueue* bq;
    std::map<int,Builder>BuilderList;

	void onFrame();
	int trackBuilder(BWAPI::Unit unit, BWAPI::UnitType buildType); //returns index to tracked builder
	CommandResult didBuilderSucceed(int key, Builder found);

};

class InitialBuildOrder{
	//just stores what to build at drone count x
	
private:
	//three vectors treated as paired by index
	std::vector<int> ibo_supplyCount; //requisite count to build unit or struct at i
	std::vector<BWAPI::UnitType> ibo_unitType; //type of unit to build
	std::vector<int> countToBuild;

	Instruction parseInstruction(const std::string& instr);

public:
	
    BuildQueue* bq;
	int curStep = -1;
	int lastStep = -1;
	int stepCount = 0;
	bool isFinished = false;

	//functions
	void nextStep(int dblSupplyCount, int* targetCount);
	int load_ibo(std::string path);
	
};



class QueueEntry{

public:
	
	bool isUpgrade = false;
	BWAPI::UpgradeType upType = BWAPI::UpgradeTypes::None;
	BWAPI::UnitType type = BWAPI::UnitTypes::None;
	int countWantedNow = -1; //-1 means not tracking
	int countBuiltNow = 0; //count built on this iteration, when count wanted equals count built reset
	int countWantedTotal = -1; //-1 means not tracking
	int countBuiltTotal = 0; //retrieve periodically
	int buildQty = 0;
	int upgradeLevel = 0;

	QueueEntry(){};
	
	QueueEntry(BWAPI::UnitType input){
		type = input;
	}

};


class BuildQueue{

private:
    Tracker track;
    InitialBuildOrder ibo = InitialBuildOrder();
    
    BuildResult ResearchUpgrade(BWAPI::UpgradeType type);
	bool BuildBuilding(BWAPI::UnitType type, BWAPI::Unit *foundBuilder, BWAPI::TilePosition desiredPos);
    BuildResult TrainUnit(BWAPI::UnitType type);
	void clearEmptyEntries();
	BWAPI::TilePosition determineLocation(BWAPI::UnitType type);
	void updateQueue();

	int updateQty(int index);
	void rmEntry(BWAPI::UnitType type);
	void rmEntry(BWAPI::UpgradeType type, int level);
	void entryToFront(BWAPI::UnitType type);

	BWAPI::UnitType queueNextPrereq(BWAPI::UnitType type);
	std::string getIboPath();

	void handleUnitEntry(QueueEntry& entry, int index);
	void handleUpgradeEntry(QueueEntry& entry, int index);

public:

	GameState *gs;
	int supplyCommited = 0;
	int minCommited = 0;
	int gasCommited = 0;
	int baseCount = 1;
	static const int maxSize = 5;
	std::vector<QueueEntry> next; //types of things we are currently trying to build
	bool onIbo = true;
	
	void onStart();
    void onFrame();

	void addEntryNow(int count, BWAPI::UnitType type);
	void replaceEntryNow(int count, BWAPI::UnitType type);
	void addEntryTotal(int count, BWAPI::UnitType type);
	void addEntryTotal(int count, BWAPI::UpgradeType type, int upgradeLevel);
	void incrementEntryTotal(int count, BWAPI::UnitType type);

	//todo more clever prioritization functions
	//note bool BWAPI::PlayerInterface::isUnitAvailable (UnitType unit) const will tell if can build
	


};