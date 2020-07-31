/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef COLLISION_RESOLUTION_SYSTEM_HPP
#define COLLISION_RESOLUTION_SYSTEM_HPP

#include <vector>
#include "../logic/elements/elem.hpp"

namespace adamant {
namespace physics {
namespace collision {

typedef std::vector<logic::elements::Elem*> Collision;

class CollisionResolutionSystem {
    public:
        static void resolve(std::vector<Collision> collisions);
    private:
        // Static class
        CollisionResolutionSystem() {}
};

}  // namespace collision
}  // namespace physics
}  // namespace adamant

#endif
