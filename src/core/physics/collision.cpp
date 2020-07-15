#include <vector>
#include <cmath>
#include "../logic/elements/elem.hpp"
#include "../logic/elements/bot.hpp"
#include "../logic/elements/ability.hpp"
#include "collision.hpp"

/* class CollisionDetectionSystem */

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

/* class CollisionResolutionSystem */

void CollisionResolutionSystem::resolve(std::vector<Collision> collisions) {
    for (auto c : collisions) {
        // Bot-bot collision
        if (c[0]->getType() == bot_t && c[1]->getType() == bot_t) {
            /* TODO: Determine which bot to move (prioritise moving bots over static ones)
             * Move the bot away from the other bot (follow the path of the bot or 
             * bounce back in the direction of the collision?)
             * What about bots which are pushed back by an ability and collide with other
             * bots, who should also be pushed back? */
        }
        // Ability-ability collision
        if (c[0]->getType() == ability_t && c[1]->getType() == ability_t) {
            // TODO: Handle specific ability-ability interactions
        }
        // Ability-bot collision
        if ((c[0]->getType() == ability_t && c[1]->getType() == bot_t) ||
            (c[1]->getType() == ability_t && c[0]->getType() == bot_t)) {
            Ability* ability = (c[0]->getType() == ability_t) ? (Ability*) c[0] : (Ability*) c[1];
            Bot* bot = (c[0]->getType() == bot_t) ? (Bot*) c[0] : (Bot*) c[1];
            ability->handleBotCollision(bot);
        }
    }
}
