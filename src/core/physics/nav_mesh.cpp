#include "nav_mesh.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

/* struct Node */

Node::Node(int x, int y, Coord o) {
    this->x = x;
    this->y = y;
    int polar_x = x - o.x;
    int polar_y = y - o.y;
    this->r = std::sqrt(std::pow(polar_x, 2) + std::pow(polar_y, 2));
    this->theta = std::atan(polar_y / polar_x);
}

bool operator < (const Node& lhs, const Node& rhs) {
    if (lhs.r != rhs.r) {
        return lhs.r < rhs.r;
    }
    return rhs.theta < rhs.theta;
}

/* class NavMesh */

/* A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
void NavMesh::triangulate(std::vector<Coord*> coords) {
    if (coords.empty()) return;

    // Initialization
    Coord o = avgCoord(coords);
    std::vector<Node*> nodes;
    for (auto coord : coords) {
        nodes.emplace_back(coord->x, coord->y, o);
    }
    std::sort(nodes.begin(), nodes.end());

    // Triangulation

    
    // Finalization
}

Coord NavMesh::avgCoord(std::vector<Coord*> coords) {
    Coord max = {0, 0};  // Assuming no negative values, as coordinates are always positive
    Coord min = {m_map_size.x, m_map_size.y};
    for (auto coord : coords) {
        if (coord->x > max.x) max.x = coord->x;
        if (coord->x < min.x) min.x = coord->x;
        if (coord->y > max.y) max.y = coord->y;
        if (coord->y < min.y) min.y = coord->y;
    }
    return {(max.x + min.x) / 2, (max.y + min.y) / 2};
}
