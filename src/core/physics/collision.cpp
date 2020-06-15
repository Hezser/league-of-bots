#include <vector>
#include <cmath>
#include "../logic/elements/elem.hpp"
#include "../logic/elements/bot.hpp"
#include "../logic/elements/ability.hpp"
#include "collision.hpp"

/* class CollisionDetectionSystem */

std::vector<Collision> CollisionDetectionSystem::detect(std::vector<Elem*> elems) {
    // Broad phase
    std::vector<Collision> suspected_collisions;
    for (auto elem1 : elems) {
        for (auto elem2 : elems) {
            if (elem1 != elem2 && (elem1->getTeam() != elem2->getTeam() || 
                        elem1->getTeam() == neutral_team ||
                        elem2->getTeam() == neutral_team)) {
                std::vector<int> coord1 = elem1->getCoord();
                std::vector<int> coord2 = elem2->getCoord();
                std::vector<int> path = {coord2[0]-coord1[0], coord2[1]-coord1[1]};
                float d = std::sqrt(std::pow(path[0], 2) + std::pow(path[1], 2));
                if (d <= (elem1->getBoundingSphereRadius() + elem2->getBoundingSphereRadius())) {
                    suspected_collisions.push_back({elem1, elem2});
                }
            }
        }
    }
    // TODO: Narrow phase: SAT or GJK
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
