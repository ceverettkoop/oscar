#pragma once
#include <BWAPI.h>


namespace Tools
{
    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units);

    int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);

    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
    BWAPI::Unitset GetUnitSetofType(BWAPI::UnitType type);
    BWAPI::Unitset GetOtherUnitSet(const BWAPI::Player me);
    BWAPI::Unit GetBuilder(BWAPI::UnitType type, BWAPI::TilePosition desiredPos);
    BWAPI::Unit GetDepot();

    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();

    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int GetTotalSupply(bool inProgress = false);
    int BuildingHowMany(BWAPI::UnitType type);

    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);
}