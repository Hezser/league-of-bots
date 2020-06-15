#include <cmath>
#include <mutex>
#include <vector>
#include "elem.hpp"

/* class elem */

Elem::Elem(ElemType type, bool alive, std::vector<int> coord, Team team, 
        int bounding_sphere_radius): m_type{type}, m_alive{alive}, m_coord{coord},
        m_team{team}, m_bounding_sphere_radius{bounding_sphere_radius} {}

ElemType Elem::getType() {
    return m_type;
}

bool Elem::isAlive() {
    return m_alive;
}

std::vector<int> Elem::getCoord() {
    return m_coord;
}

void Elem::setCoord(std::vector<int> coord) {
    m_coord = coord;
}

Team Elem::getTeam() {
    return m_team;
}

int Elem::getBoundingSphereRadius() {
    return m_bounding_sphere_radius;
}

void Elem::setBoundingSphereRadius(int radius) {
    m_bounding_sphere_radius = radius;
}

void Elem::kill() {
    m_alive = false;
}
