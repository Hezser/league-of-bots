/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef LINEAR_MOVE_HPP
#define LINEAR_MOVE_HPP

#include "move.hpp"
#include "../graphics/coord.hpp"

namespace adamant {
namespace physics {
namespace movement {

typedef struct LinearMove: public Move {
    LinearMove(graphics::Coord start, graphics::Coord target, MovePriority priority);
} LinearMove;

}  // namespace movement
}  // namespace physics
}  // namespace adamant

#endif
