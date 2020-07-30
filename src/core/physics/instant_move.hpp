#ifndef INSTANT_MOVE_HPP
#define INSTANT_MOVE_HPP

#include "move.hpp"
#include "../graphics/elements/coord.hpp"

typedef struct InstantMove: public Move {
    InstantMove(Coord start, Coord target, MovePriority priority);
} InstantMove;

#endif
