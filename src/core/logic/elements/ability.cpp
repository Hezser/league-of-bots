#include <vector>
#include <chrono>
#include <cmath>
#include "ability.hpp"
#include "elem.hpp"
#include "bot.hpp"
#include "../../physics/movement.hpp"

/* class Ability */

Ability::Ability(ElemType type, bool alive, std::vector<int> coord, Team team,
        Bot* bot, time_t cd): Elem(type, alive, coord, team), m_bot{bot}, m_cd{cd},
        m_last_used{std::chrono::steady_clock::time_point(std::chrono::seconds(0))} {}

time_t Ability::getCd() {
    return m_cd;
}

void Ability::setCd(time_t cd) {
    m_cd = cd;
}

std::chrono::steady_clock::time_point Ability::getLastUsed() {
    return m_last_used;
}

/* class SaiQAbility */

SaiQAbility::SaiQAbility(Bot* bot): Ability(ability_t, false, {0,0}, bot->getTeam(),
        bot, 2000), m_movement_manager{new MovementManager(this, 3.00)} {}

bool SaiQAbility::cast(std::vector<int> target) {
    m_bot->mutex.lock();
    std::vector<int> start = m_bot->getCoord();
    m_bot->mutex.unlock();
    this->m_coord = start;
    Move* move = constructLinearMove(start, target, own_ability);
    mutex.lock();
    m_movement_manager->request(move);
    m_alive = true;
    mutex.unlock();
    return true;
}

void SaiQAbility::update(double ms) {
    if (!m_movement_manager->update(ms)) {
        m_alive = false;
    }
    // More stuff...
}

/* class SaiWAbility */

SaiWAbility::SaiWAbility(Bot* bot): Ability(ability_t, false, {0,0}, bot->getTeam(),
        bot, 2000) {}

bool SaiWAbility::cast(std::vector<int> target) {
    return true; 
}

void SaiWAbility::update(double ms) {

}

/* class SaiEAbility */

SaiEAbility::SaiEAbility(Bot* bot): Ability(ability_t, false, {0,0}, bot->getTeam(),
        bot, 2000) {}

bool SaiEAbility::cast(std::vector<int> target) {
    return true; 
}

void SaiEAbility::update(double ms) {
    
}

/* class SaiRAbility */

SaiRAbility::SaiRAbility(Bot* bot): Ability(ability_t, false, {0,0}, bot->getTeam(),
        bot, 2000) {}

bool SaiRAbility::cast(std::vector<int> target) {
    return true; 
}

void SaiRAbility::update(double ms) {

}
