#include <chrono>
#include <vector>
#include <cmath>
#include <iostream>
#include "bot.hpp"
#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/movement.hpp"

/* class bot */

Bot::Bot(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team, 
        int bounding_sphere_radius, MovementManager* movement_manager): 
        Elem(type, alive, shape, center, team, bounding_sphere_radius), 
        m_movement_manager{movement_manager} {}

void Bot::moveTowards(Coord target) {
    mutex.lock();
    Move* move = constructLinearMove(m_shape->center, target, right_click);
    m_movement_manager->request(move);
    mutex.unlock();
}

void Bot::moveTo(Coord target) {
    mutex.lock();
    Move* move = constructInstantMove(m_shape->center, target, right_click);
    m_movement_manager->request(move);
    mutex.unlock();
}

Ability* Bot::useAbility(AbilityKey key, Coord target) {
    Ability* ability = nullptr;
    switch(key) {
        case q:
            ability = this->m_abilities[0];
            break;
        case w:
            ability = this->m_abilities[1];
            break;
        case e:
            ability = this->m_abilities[2];
            break;
        case r:
            ability = this->m_abilities[3];
            break;
        default:
            break;
    }
    if (ability != nullptr && ability->cast(target)) {
        return ability;
    }
    return nullptr;
}

/* class sai_bot */

SaiBot::SaiBot(Team team, Coord start): Bot(bot_t, true, new ConvexPolygon({{0,0}, {0,50},
            {50,50}, {50, 0}}), start, team, 14, new MovementManager(this, 1.0f)) {
    /* We initialize abilities after the bot is fully initialized so that the Ability
     * constructor can use the bot's members */
    m_abilities = {new SaiQAbility(this), new SaiWAbility(this), new SaiEAbility(this), 
                   new SaiRAbility(this)};
}

void SaiBot::update(float ms) {
    m_movement_manager->update(ms);
    // In the future, health, mana, experience, etc. will also need to be updated here
}
