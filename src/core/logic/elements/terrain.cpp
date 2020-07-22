#include <vector>
#include "terrain.hpp"
#include "elem.hpp"

/* class Terrain */

Terrain::Terrain(ConvexPolygon* shape, Coord center, int bounding_sphere_radius):
        Elem(terrain_t, true, shape, center, neutral_team, bounding_sphere_radius) {}

void Terrain::update(float ms) {
    // TODO: Update animations
}

