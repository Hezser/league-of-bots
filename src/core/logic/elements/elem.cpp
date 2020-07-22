#include <cmath>
#include <mutex>
#include <vector>
#include <iostream>
#include "elem.hpp"

/* class elem */

Elem::Elem(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team, 
        int bounding_sphere_radius): m_type{type}, m_alive{alive}, m_shape{shape},
        m_team{team}, m_bounding_sphere_radius{bounding_sphere_radius} {
    m_shape->center = center;
}

ElemType Elem::getType() {
    return m_type;
}

bool Elem::isAlive() {
    return m_alive;
}

ConvexPolygon* Elem::getShape() {
    return m_shape;
}

void Elem::setShape(ConvexPolygon* shape) {
    m_shape = shape;
}

Coord Elem::getCenter() {
    return m_shape->center;
}

void Elem::setCenter(Coord center) {
    m_shape->center = center;
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
    std::cout << m_team << std::endl;
    m_alive = false;
}
