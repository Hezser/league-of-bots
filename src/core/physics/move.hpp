#ifndef MOVE_HPP
#define MOVE_HPP

#include "../graphics/elements/coord.hpp"
#include <vector>

typedef enum MovePriority {
    right_click = 0, own_ability = 1, enemy_ability = 2
} MovePriority;

typedef struct Move {
    Coord start;
    Coord target;
    std::vector<float> unit_travel;
    int distance;
    int travelled;
    MovePriority priority;

    friend bool operator < (const Move& lhs, const Move& rhs);
    friend bool operator > (const Move& lhs, const Move& rhs);

    struct GreaterComparator {
        bool operator() (const Move* lhs, const Move* rhs);
    };
} Move;

#endif
