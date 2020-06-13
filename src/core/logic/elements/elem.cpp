#include <cmath>
#include <mutex>
#include <vector>
#include "elem.hpp"

/* class elem */

Elem::Elem(ElemType type, bool alive, std::vector<int> coord, Team team): m_type{type}, 
        m_alive{alive}, m_coord{coord}, m_team{team} {}

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
