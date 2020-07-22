#include <vector>
#include <chrono>
#include <cmath>
#include <iostream>
#include "ability.hpp"
#include "elem.hpp"
#include "bot.hpp"
#include "../../physics/movement.hpp"

/* class Ability */

Ability::Ability(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team,
        Bot* bot, time_t cd, int bounding_sphere_radius): Elem(type, alive, shape, center,
        team, bounding_sphere_radius), m_bot{bot}, m_cd{cd},
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

SaiQAbility::SaiQAbility(Bot* bot): Ability(ability_t,
        false, new ConvexPolygon({{0,0}, {0,5}, {5,5}, {5,0}}), {0,0}, bot->getTeam(), bot,
        2000, 5), m_movement_manager{new MovementManager(this, 2.0f)} {}

bool SaiQAbility::cast(Coord target) {
    m_bot->mutex.lock();
    Coord start = m_bot->getCenter();
    m_bot->mutex.unlock();
    m_shape->center = start;
    Move* move = constructLinearMove(start, target, own_ability);
    mutex.lock();
    m_movement_manager->request(move);
    m_alive = true;
    mutex.unlock();
    return true;
}

void SaiQAbility::update(float ms) {
    if (!m_movement_manager->update(ms)) {
        m_alive = false;
    }
    // More stuff...
}

void SaiQAbility::handleBotCollision(Bot* bot) {
    if (bot->getTeam() != m_team) {
        bot->kill();
    }
}

/* class SaiWAbility */

SaiWAbility::SaiWAbility(Bot* bot): Ability(ability_t, false,
        new ConvexPolygon(std::vector<Coord>()), {0,0}, bot->getTeam(), bot, 2000, 0) {}

bool SaiWAbility::cast(Coord target) {
    return true; 
}

void SaiWAbility::update(float ms) {

}

void SaiWAbility::handleBotCollision(Bot* bot) {

}

/* class SaiEAbility */

SaiEAbility::SaiEAbility(Bot* bot): Ability(ability_t, false,
        new ConvexPolygon(std::vector<Coord>()), {0,0}, bot->getTeam(), bot, 2000, 0) {}

bool SaiEAbility::cast(Coord target) {
    return true; 
}

void SaiEAbility::update(float ms) {
    
}

void SaiEAbility::handleBotCollision(Bot* bot) {

}

/* class SaiRAbility */

SaiRAbility::SaiRAbility(Bot* bot): Ability(ability_t, false,
        new ConvexPolygon(std::vector<Coord>()), {0,0}, bot->getTeam(), bot, 2000, 0) {}

bool SaiRAbility::cast(Coord target) {
    return true; 
}

void SaiRAbility::update(float ms) {

}

void SaiRAbility::handleBotCollision(Bot* bot) {

}
