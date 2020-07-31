/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef INSTANT_MOVE_HPP
#define INSTANT_MOVE_HPP

#include "move.hpp"
#include "../graphics/coord.hpp"

namespace adamant {
namespace physics {
namespace movement {

typedef struct InstantMove: public Move {
    InstantMove(graphics::Coord start, graphics::Coord target, MovePriority priority);
} InstantMove;

}  // namespace movement
}  // namespace physics
}  // namespace adamant

#endif
