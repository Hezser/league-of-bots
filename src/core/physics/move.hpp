/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef MOVE_HPP
#define MOVE_HPP

#include "../graphics/coord.hpp"
#include <vector>

namespace adamant {
namespace physics {
namespace movement {

typedef enum MovePriority {
    right_click = 0, own_ability = 1, enemy_ability = 2
} MovePriority;

typedef struct Move {
    graphics::Coord start;
    graphics::Coord target;
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

}  // namespace movement
}  // namespace physics
}  // namespace adamant

#endif
