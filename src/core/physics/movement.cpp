#include <chrono>
#include <vector>
#include <queue>
#include <iostream>
#include <cmath>
#include "movement.hpp"

/* Helpers */

Move* constructLinearMove(std::vector<int> start, std::vector<int> target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    std::vector<float> alpha_target = {(float)target[0] - start[0], (float)target[1] - start[1      ]};
    // Normalize the alpha vector
    int d = sqrt(pow(alpha_target[0], 2) + pow(alpha_target[1], 2));
    std::vector<float> unit_travel = {alpha_target[0] / d, alpha_target[1] / d};
    Move* move = new Move{start, target, unit_travel, d, 0, priority};
    return move;
}

Move* constructInstantMove(std::vector<int> start, std::vector<int> target, MovePriority priority) {
    // Convert the target vector to a basis where the start is the origin
    std::vector<float> alpha_target = {(float)target[0] - start[0], (float)target[1] - start[1      ]};
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

bool MovementManager::update(double ms) {
    while(!m_moves.empty()) {
        // Get the highest priority move
        Move* move = m_moves.top();
        // The move has finished
        if (move->travelled == move->distance) {
            m_moves.pop();
            continue;
        }
        // Calculate next coord
        int x = (int) (m_velocity * (move->travelled+1) * move->unit_travel[0]);
        int y = (int) (m_velocity * (move->travelled+1) * move->unit_travel[1]);
        int prev_x = (int) (m_velocity * move->travelled * move->unit_travel[0]);
        int prev_y = (int) (m_velocity * move->travelled * move->unit_travel[1]);
        // Update the elem's coord
        int step_x = x - prev_x;
        int step_y = y - prev_y;
        m_elem->mutex.lock();
        std::vector<int> coord = m_elem->getCoord();
        x = coord[0] + (ms * (step_x));
        y = coord[1] + (ms * (step_y));
        m_elem->setCoord({x, y});
        m_elem->mutex.unlock();
        // Update the move
        move->travelled += std::sqrt(std::pow(step_x, 2) + std::pow(step_y, 2));
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
