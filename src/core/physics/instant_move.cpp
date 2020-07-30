/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "instant_move.hpp"

InstantMove::InstantMove(Coord start, Coord target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    this->start = start;
    this->target = target;
    this->unit_travel = {(float)target.x - start.x, (float)target.y - start.y};
    this->distance = 1;
    this->travelled = 0;
    this->priority = priority;
}
