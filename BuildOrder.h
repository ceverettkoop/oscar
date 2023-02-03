#pragma once


#include <BWAPI.h>
#include <vector>


class InitialBuildOrder{
	//just stores what to build at drone count x
	std::vector<int> ibo_supplyCount; //requisite count to build unit or struct at i
	std::vector<BWAPI::UnitTypes::Enum::Enum> ibo_unitType; //type of unit to build

public:
	InitialBuildOrder(); //constructor

	bool isFinished(int supplyCount);
	BWAPI::UnitTypes::Enum::Enum InitialBuildOrder::nextStep(int supplyCount, int* targetCount);
	int load_ibo(std::string* path);
};