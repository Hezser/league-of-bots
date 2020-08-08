/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "movement_manager.hpp"
#include <iostream>

using namespace adamant::physics::movement;
using namespace adamant::logic::elements;
using namespace adamant::graphics;

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
        float prev_x = move->travelled * move->unit_travel[0];
        float prev_y = move->travelled * move->unit_travel[1];
        float d = std::min(m_velocity * ms, move->distance - move->travelled);
        move->travelled += d;
        // Calculate next coord
        float this_x = move->travelled * move->unit_travel[0];
        float this_y = move->travelled * move->unit_travel[1];
        int step_x = (int) this_x - (int) prev_x;
        int step_y = (int) this_y - (int) prev_y;
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

void MovementManager::MovePriorityQueue::removeRightClickMove() {
    for (int i=0; i<c.size(); i++) {
        if (c[i]->priority == right_click) {
            c.erase(c.begin()+i);
            break;
        }
    }
}

void MovementManager::MovePriorityQueue::replaceRightClickMove(Move* move) {
    removeRightClickMove();
    push(move);
}
