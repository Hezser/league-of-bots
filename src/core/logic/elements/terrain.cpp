/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "terrain.hpp"
#include "elem.hpp"
#include <vector>

using namespace adamant::logic::elements;
using namespace adamant::graphics;
using namespace adamant::graphics::elements;

Terrain::Terrain(ConvexPolygon* shape, Coord center, int bounding_sphere_radius):
        Elem(terrain_t, true, shape, center, neutral_team, bounding_sphere_radius) {}

void Terrain::update(float ms) {
    // TODO: Update animations
}

