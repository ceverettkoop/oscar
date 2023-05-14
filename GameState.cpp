#include "GameState.h"
#include "Tools.h"

void GameState::onStart() {

    m_me = BWAPI::Broodwar->self();
    m_enemy = BWAPI::Broodwar->enemy();
    m_selfRace = BWAPI::Broodwar->self()->getRace();
    m_enemyRace = BWAPI::Broodwar->enemy()->getRace();
    m_map.onStart();
}

void GameState::MapOnFrame() {
    m_map.onFrame();
}

void GameState::UpdateUnits() {

    m_myUnits = me->getUnits();
    m_otherUnits = Tools::GetOtherUnitSet(me);


}
