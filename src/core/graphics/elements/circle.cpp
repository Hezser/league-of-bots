/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "circle.hpp"

Circle::Circle(Coord center, int radius): Shape(circle, center) {
    this->radius = radius;
    sf::CircleShape* d = new sf::CircleShape();
    d->setRadius(radius);
    this->drawable = d;
}

sf::Shape* Circle::getDrawable() {
    drawable->setPosition(m_center.x - radius, m_center.y - radius);
    return drawable;
}

