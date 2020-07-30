#include "shape.hpp"

Shape::Shape(ShapeType type) {
    this->type = type;
}

Shape::Shape(ShapeType type, Coord center): Shape(type) {
    this->m_center = center;
}

Coord Shape::getCenter() {
    return m_center;
}

void Shape::setCenter(Coord center) {
    m_center = center;
}

