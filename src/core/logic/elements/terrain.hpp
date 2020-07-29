#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "elem.hpp"

class Terrain: public Elem {
    public:
        Terrain(ConvexPolygon* shape, Coord center, int bounding_sphere_radius);
        void update(float ms) override;
};

#endif
