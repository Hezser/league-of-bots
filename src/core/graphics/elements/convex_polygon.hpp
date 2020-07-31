/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef CONVEX_POLYGON_HPP
#define CONVEX_POLYGON_HPP

#include "../coord.hpp"
#include "polygon.hpp"
#include <vector>

namespace adamant {
namespace graphics {
namespace elements {

class ConvexPolygon: public Polygon {
    public:
        ConvexPolygon(std::vector<Coord> coords);

    protected:
        ConvexPolygon(ShapeType subtype);  // Used for instantiating subclasses
        void constructDrawable(std::vector<Coord> coords);
        std::vector<Node*> createNodes(std::vector<Coord> coords);
};

}  // namespace elements
}  // namespace graphics
}  // namespace adamant

#endif
