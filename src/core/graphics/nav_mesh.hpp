/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef NAV_MESH_HPP
#define NAV_MESH_HPP

#include <vector>
#include <exception>
#include "map_size.hpp"
#include "./coord.hpp"
#include "./elements/node.hpp"
#include "./elements/triangle.hpp"
#include "./elements/hull.hpp"
#include "../logic/elements/terrain.hpp"

namespace adamant {
namespace graphics {

using TriangleMesh = std::vector<graphics::elements::Triangle*>;

class NavMesh {
    const MapSize m_map_size;
    TriangleMesh m_mesh;
    Coord m_origin;
    std::vector<graphics::elements::Node*> m_nodes;

    void drawFirstTriangle(std::vector<graphics::elements::Node*>& nodes, graphics::elements::Hull& frontier);
    graphics::elements::Triangle* legalize(graphics::elements::Triangle* candidate);
    void sideWalk(graphics::elements::Edge* edge, graphics::elements::Edge* neighbour, graphics::elements::Hull& frontier);
    void finalWalk(graphics::elements::Edge* initial_edge, graphics::elements::Hull& frontier);
    void triangulate(std::vector<logic::elements::Terrain*>& terrains);
    void removeTrianglesWithin(std::vector<logic::elements::Terrain*>& terrains);
    void populateNodes();
    Coord avgCoord(std::vector<graphics::elements::Node*>& nodes) const;

    public:
        // May throw InsufficientNodesException or FailedTriangulationException
        NavMesh(std::vector<logic::elements::Terrain*> terrains, MapSize map_size);
        MapSize getMapSize() const;
        TriangleMesh getMesh() const;
        std::vector<elements::Node*> getNodes() const;

        struct InsufficientNodesException: public std::exception {
            const char* what() const noexcept;
        };

        struct FailedTriangulationException: public std::exception {
            const char* what() const noexcept;
        };
};

}  // namespace graphics
}  // namespace adamant

#endif
