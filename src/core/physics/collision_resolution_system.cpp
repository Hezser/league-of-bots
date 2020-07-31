/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "collision_resolution_system.hpp"
#include "../logic/elements/bot.hpp"
#include "../logic/elements/ability.hpp"

using namespace adamant::physics::collision;
using namespace adamant::logic::elements;

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
