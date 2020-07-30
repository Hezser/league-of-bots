#ifndef CONVEX_POLYGON_HPP
#define CONVEX_POLYGON_HPP

#include "coord.hpp"
#include "polygon.hpp"
#include <vector>

class ConvexPolygon: public Polygon {
    public:
        ConvexPolygon(std::vector<Coord> coords);

    protected:
        ConvexPolygon(ShapeType subtype);  // Used for instantiating subclasses
        void constructDrawable(std::vector<Coord> coords);
        std::vector<Node*> createNodes(std::vector<Coord> coords);
};

#endif
