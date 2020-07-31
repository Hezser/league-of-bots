/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef BOT_HPP
#define BOT_HPP

#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/movement_manager.hpp"
#include <chrono>
#include <vector>

namespace adamant {
namespace logic {
namespace elements {

class Bot: public Elem {
    public:
        void moveTowards(graphics::Coord target);
        void moveTo(graphics::Coord target);
        Ability* useAbility(AbilityKey key, graphics::Coord target);
        virtual void update(float ms) = 0;

    protected:
        Bot();
        Bot(ElemType type, bool alive, graphics::elements::ConvexPolygon* shape, 
                graphics::Coord center, Team team, int bounding_sphere_radius,
                physics::movement::MovementManager* movement_manager);
        std::vector<Ability*> m_abilities;
        physics::movement::MovementManager* m_movement_manager;
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
