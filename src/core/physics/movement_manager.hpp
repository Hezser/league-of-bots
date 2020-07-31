/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef MOVEMENT_MANAGER_HPP
#define MOVEMENT_MANAGER_HPP

#include <queue>
#include <vector>
#include "move.hpp"
#include "../logic/elements/elem.hpp"

namespace adamant {
namespace physics {
namespace movement {

class MovementManager {
    public:
        MovementManager(logic::elements::Elem* elem, float velocity);
        bool update(float ms);
        void request(Move* move);
        float getVelocity();
        void setVelocity(float velocity);

    private:
        struct MovePriorityQueue: public std::priority_queue<Move*, std::vector<Move*>, 
                Move::GreaterComparator> {
            public:
                void removeRightClickMove();
                void replaceRightClickMove(Move* move);
        };

        logic::elements::Elem* m_elem;
        MovePriorityQueue m_moves;
        float m_velocity;
};

}  // namespace movement
}  // namespace physics
}  // namespace adamant

#endif
