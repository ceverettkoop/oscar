#pragma once


#include <BWAPI.h>
#include <vector>


class InitialBuildOrder{
	//just stores what to build at drone count x
	std::vector<int> ibo_supplyCount; //requisite count to build unit or struct at i
	std::vector<BWAPI::UnitType> ibo_unitType; //type of unit to build
	int stepCount;
public:
	InitialBuildOrder(); //constructor

	bool isFinished(int supplyCount);
	BWAPI::UnitType nextStep(int supplyCount, int* targetCount);
	int load_ibo(std::string* path);
};



/*
store next thing to build in static variable

every frame
if else tree:
are we on initial build order (static bool)
	if yes nextStep
		if supplyCount = targetCount
			next = unit type
		else build probe
else (not on initial build order)
	some decision function????



*/