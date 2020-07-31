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

typedef std::vector<graphics::elements::Triangle*> TriangleMesh;

class NavMesh {
    public:
        // May throw InsufficientNodesException or FailedTriangulationException
        NavMesh(std::vector<logic::elements::Terrain*> terrains, MapSize map_size);
        MapSize getMapSize();
        TriangleMesh getMesh();
        std::vector<elements::Node*> getNodes();

        struct InsufficientNodesException: public std::exception {
            const char* what() const throw();
        };

        struct FailedTriangulationException: public std::exception {
            const char* what() const throw();
        };

    private:
        MapSize m_map_size;
        TriangleMesh m_mesh;
        Coord m_origin;
        std::vector<graphics::elements::Node*> m_nodes;
        graphics::elements::Triangle* legalize(graphics::elements::Triangle* candidate, 
                graphics::elements::Hull* frontier);
        void triangulate(std::vector<logic::elements::Terrain*> terrains);
        void populateNodes();
        Coord avgCoord(std::vector<graphics::elements::Node*> nodes);
};

}  // namespace graphics
}  // namespace adamant

#endif
