/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

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

