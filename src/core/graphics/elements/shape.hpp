/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "../coord.hpp"
#include <SFML/Graphics.hpp>

namespace adamant {
namespace graphics {
namespace elements {

class Shape {
    public:
        typedef enum ShapeType {
            circle = 0, polygon = 1, convex_polygon = 2, triangle = 3
        } ShapeType;

        ShapeType type;
        Coord getCenter();
        virtual void setCenter(Coord center);
        virtual sf::Shape* getDrawable() = 0;
        // TODO: Functions to modify drawable (color, outline, etc)
    
    protected:
        Shape(ShapeType type, Coord center);
        Shape(ShapeType type);  // Used for instantiating subclasses
        Coord m_center;
        sf::Shape* drawable;
};

}  // namespace elements
}  // namespace graphics
}  // namespace adamant

#endif
