#include <chrono>
#include <vector>
#include <cmath>
#include <iostream>
#include "bot.hpp"
#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/movement.hpp"

/* class bot */

Bot::Bot(ElemType type, bool alive, std::vector<int> coord, Team team, int bounding_sphere_radius,
        std::vector<Ability*> abilities, MovementManager* movement_manager):
        Elem(type, alive, coord, team, bounding_sphere_radius),
        m_abilities{abilities}, m_movement_manager{movement_manager} {}

void Bot::moveTowards(std::vector<int> target) {
    mutex.lock();
    Move* move = constructLinearMove(m_coord, target, right_click);
    m_movement_manager->request(move);
    mutex.unlock();
}

void Bot::moveTo(std::vector<int> target) {
    mutex.lock();
    Move* move = constructInstantMove(m_coord, target, right_click);
    m_movement_manager->request(move);
    mutex.unlock();
}

Ability* Bot::useAbility(AbilityKey key, std::vector<int> target) {
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

SaiBot::SaiBot(Team team, std::vector<int> start): Bot(bot_t, true, start, team, 14,
        {new SaiQAbility(this), new SaiWAbility(this), new SaiEAbility(this), 
        new SaiRAbility(this)}, new MovementManager(this, 1.0f)) {}

void SaiBot::update(float ms) {
    m_movement_manager->update(ms);
    // In the future, health, mana, experience, etc. will also need to be updated here
}
