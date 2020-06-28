#include <vector>
#include "terrain.hpp"
#include "elem.hpp"

/* class Terrain */

Terrain::Terrain(Coord coord, int bounding_sphere_radius): Elem(terrain_t,
        true, coord, neutral_team, bounding_sphere_radius) {}

void Terrain::update(float ms) {
    // TODO: Update animations
}

