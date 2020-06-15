#include <chrono>
#include <vector>
#include <queue>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <limits>
#include "movement.hpp"

/* Helpers */

Move* constructLinearMove(std::vector<int> start, std::vector<int> target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    std::vector<float> alpha_target = {(float)target[0] - start[0], (float)target[1] - start[1]};
    // Normalize the alpha vector
    int d = std::round(sqrt(pow(alpha_target[0], 2) + pow(alpha_target[1], 2)));
    std::vector<float> unit_travel = {alpha_target[0] / d, alpha_target[1] / d};
    Move* move = new Move{start, target, unit_travel, d, 0, priority};
    return move;
}

Move* constructInstantMove(std::vector<int> start, std::vector<int> target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    std::vector<float> alpha_target = {(float)target[0] - start[0], (float)target[1] - start[1]};
    Move* move = new Move{start, target, alpha_target, 1, 0, priority};
    return move;
}

/* struct Move */

bool operator < (const Move& lhs, const Move& rhs) {
    return lhs.priority < rhs.priority;
}

bool operator > (const Move& lhs, const Move& rhs) {
    return lhs.priority > rhs.priority;
}

bool Move::GreaterComparator::operator() (const Move* lhs, const Move* rhs) {   
    return lhs->priority > rhs->priority;                   
}

/* class MovePriorityQueue */

void MovePriorityQueue::removeRightClickMove() {
    for (int i=0; i<c.size(); i++) {
        if (c[i]->priority == right_click) {
            c.erase(c.begin()+i);
            break;
        }
    }
}

void MovePriorityQueue::replaceRightClickMove(Move* move) {
    removeRightClickMove();
    push(move);
}

/* class MovementManager */

MovementManager::MovementManager(Elem* elem, float velocity): m_elem{elem},
        m_velocity{velocity} {}

bool MovementManager::update(float ms) {
    while(!m_moves.empty()) {
        // Get the highest priority move
        Move* move = m_moves.top();
        // The move has finished
        if (move->travelled == move->distance) {
            m_moves.pop();
            continue;
        }
        // Update the move
        int d = std::min((int) std::round(m_velocity * ms), move->distance - move->travelled);
        move->travelled += d;
        // Calculate next coord
        int x = (int) (move->travelled * move->unit_travel[0]);
        int y = (int) (move->travelled * move->unit_travel[1]);
        int prev_x = (int) ((move->travelled-d) * move->unit_travel[0]);
        int prev_y = (int) ((move->travelled-d) * move->unit_travel[1]);
        int step_x = x - prev_x;
        int step_y = y - prev_y;
        // Update the elem's coord
        m_elem->mutex.lock();
        std::vector<int> coord = m_elem->getCoord();
        m_elem->setCoord({coord[0] + step_x, coord[1] + step_y}); 
        m_elem->mutex.unlock();
        return true;
    }
    return false;
}

void MovementManager::request(Move* move) {
    // TODO: ideally, override push() in MovePriorityQueue
    if (move->priority == right_click) {
        m_moves.replaceRightClickMove(move);
    } else {
        m_moves.push(move);
    }      
}

float MovementManager::getVelocity() {
    return this->m_velocity;
}

void MovementManager::setVelocity(float velocity) {
    this->m_velocity = velocity;
}
