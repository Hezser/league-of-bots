/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "collision_detection_system.hpp"

std::vector<Collision> CollisionDetectionSystem::detect(std::vector<Elem*> elems) {
    // TODO: Collidables must be a class optimized for collision detection, like in Havok
    // Copy of vector elems
    std::vector<Elem*> collidables = elems;
    // Broad phase
    std::vector<Collision> suspected_collisions;
    while(!collidables.empty()) {
        Elem* c1 = collidables[0];
        collidables.erase(collidables.begin());
        for (auto c2 : collidables) {
            if (c1 != c2 && (c1->getTeam() != c2->getTeam() || 
                        c1->getTeam() == neutral_team ||
                        c2->getTeam() == neutral_team)) {
                Coord coord1 = c1->getCenter();
                Coord coord2 = c2->getCenter();
                std::vector<int> path = {coord2.x-coord1.x, coord2.y-coord1.y};
                float d = std::sqrt(std::pow(path[0], 2) + std::pow(path[1], 2));
                if (d <= (c1->getBoundingSphereRadius() + c2->getBoundingSphereRadius())) {
                    suspected_collisions.push_back({c1, c2});
                }
            }
        }
    }
    // TODO: Narrow phase: SAT or GJK over suspected_collisions
    return suspected_collisions;
}

