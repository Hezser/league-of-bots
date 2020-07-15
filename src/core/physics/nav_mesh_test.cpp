#include <vector>
#include "nav_mesh.hpp"
#include "shape.hpp"
#include "../logic/elements/terrain.hpp"

int main() {
    // Create terrain
    MapSize map_size = {1920, 1080};
    std::vector<Terrain*> terrains;
    for (auto i=0; i<map_size.y; i += 200) {
        ConvexPolygon* s = new ConvexPolygon({{i, i}, {i, i+50}, {i+50, i+50}, {i+50, i}});
        Terrain* t = new Terrain(s, {i+25, i+25}, 71);
        terrains.push_back(t);
    }
    
    // Triangulate map

    // Display triangulation

    return 0;
}
