#ifndef MOVEMENT_MANAGER_HPP
#define MOVEMENT_MANAGER_HPP

#include <queue>
#include <vector>
#include "move.hpp"
#include "../logic/elements/elem.hpp"

class MovementManager {
    public:
        MovementManager(Elem* elem, float velocity);
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

        Elem* m_elem;
        MovePriorityQueue m_moves;
        float m_velocity;
};

#endif
