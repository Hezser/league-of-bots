#include "linear_move.hpp"
#include <cmath>

LinearMove::LinearMove(Coord start, Coord target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    std::vector<float> alpha_target = {(float)target.x - start.x, (float)target.y - start.y};
    // Normalize the alpha vector
    int d = std::round(sqrt(pow(alpha_target[0], 2) + pow(alpha_target[1], 2)));
    std::vector<float> unit_travel = {alpha_target[0] / d, alpha_target[1] / d};
    this->start = start;
    this->target = target;
    this->unit_travel = unit_travel;
    this->distance = d;
    this->travelled = 0;
    this->priority = priority;
}

