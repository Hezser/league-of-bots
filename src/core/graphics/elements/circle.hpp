/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef CIRCLE_HPP
#define CIRCLE_HPP

#include "coord.hpp"
#include "shape.hpp"
#include <SFML/Graphics.hpp>

/* TODO: Delete? 
 * Cannot be used for elem shapes, as circles do not have edges or nodes to triangulate,
 * check for collisions, etc */
class Circle: public Shape {
    public:
        int radius;
        Circle(Coord center, int radius);
        sf::Shape* getDrawable() override;

    private:
        Circle();
};

#endif

