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

