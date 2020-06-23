#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "elem.hpp"

class Terrain: public Elem {
    public:
        Terrain(std::vector<int> coord, int bounding_sphere_radius);
        void update(float ms) override;
};

#endif
