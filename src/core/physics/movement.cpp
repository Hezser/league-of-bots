#include <chrono>
#include <vector>
#include <queue>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <limits>
#include "movement.hpp"

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

/* Helpers */

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

InstantMove::InstantMove(Coord start, Coord target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    this->start = start;
    this->target = target;
    this->unit_travel = {(float)target.x - start.x, (float)target.y - start.y};
    this->distance = 1;
    this->travelled = 0;
    this->priority = priority;
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
        Coord coord = m_elem->getCenter();
        m_elem->setCenter({coord.x + step_x, coord.y + step_y}); 
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
