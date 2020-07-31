/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include "elem.hpp"

namespace adamant {
namespace logic {
namespace elements {

class Terrain: public Elem {
    public:
        Terrain(graphics::elements::ConvexPolygon* shape, graphics::Coord center,
                int bounding_sphere_radius);
        void update(float ms) override;
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
