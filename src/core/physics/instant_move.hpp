/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef INSTANT_MOVE_HPP
#define INSTANT_MOVE_HPP

#include "move.hpp"
#include "../graphics/elements/coord.hpp"

typedef struct InstantMove: public Move {
    InstantMove(Coord start, Coord target, MovePriority priority);
} InstantMove;

#endif
