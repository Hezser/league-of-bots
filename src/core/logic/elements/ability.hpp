/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef ABILITY_HPP
#define ABILITY_HPP

#include <vector>
#include <chrono>
#include "elem.hpp"
#include "../../physics/movement_manager.hpp"

namespace adamant {
namespace logic {
namespace elements {

// Forward declaration
class Bot;

typedef enum AbilityKey {
    q, w, e, r
} AbilityKey;

class Ability: public Elem {
    public:
        time_t getCd();
        void setCd(time_t cd);
        std::chrono::steady_clock::time_point getLastUsed();
        virtual bool cast(graphics::Coord target) = 0;
        virtual void update(float ms) = 0;
        virtual void handleBotCollision(Bot* bot) = 0;

    protected:
        Bot* m_bot;
        time_t m_cd;
        std::chrono::steady_clock::time_point m_last_used;
        Ability();
        Ability(ElemType type, bool alive, graphics::elements::ConvexPolygon* shape, 
                graphics::Coord center, Team team, Bot* bot, time_t cd,
                int bounding_sphere_radius);
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
