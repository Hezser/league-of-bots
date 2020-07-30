/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef COLLISION_DETECTION_SYSTEM_HPP
#define COLLISION_DETECTION_SYSTEM_HPP

#include "../logic/elements/elem.hpp"
#include <vector>

typedef std::vector<Elem*> Collision;

class CollisionDetectionSystem {
    public:
        // Meant to be used a posteriori
        static std::vector<Collision> detect(std::vector<Elem*> elems);
    private:
        // Static class
        CollisionDetectionSystem() {}
};

#endif
