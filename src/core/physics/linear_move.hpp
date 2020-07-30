/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef LINEAR_MOVE_HPP
#define LINEAR_MOVE_HPP

#include "move.hpp"
#include "../graphics/elements/coord.hpp"

typedef struct LinearMove: public Move {
    LinearMove(Coord start, Coord target, MovePriority priority);
} LinearMove;

#endif
