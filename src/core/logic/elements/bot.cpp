/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "bot.hpp"
#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/linear_move.hpp"
#include "../../physics/instant_move.hpp"
#include <chrono>
#include <vector>
#include <cmath>
#include <iostream>

using namespace adamant::logic::elements;
using namespace adamant::graphics;
using namespace adamant::graphics::elements;
using namespace adamant::physics::movement;

Bot::Bot(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team, 
        int bounding_sphere_radius, MovementManager* movement_manager): 
        Elem(type, alive, shape, center, team, bounding_sphere_radius), 
        m_movement_manager{movement_manager} {}

void Bot::moveTowards(Coord target) {
    mutex.lock();
    Move* move = new LinearMove(m_shape->getCenter(), target, right_click);
    m_movement_manager->request(move);
    mutex.unlock();
}

void Bot::moveTo(Coord target) {
    mutex.lock();
    Move* move = new InstantMove(m_shape->getCenter(), target, right_click);
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
