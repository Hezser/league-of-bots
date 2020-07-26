#ifndef NAV_MESH_HPP
#define NAV_MESH_HPP

#include <vector>
#include <exception>
#include "shape.hpp"
#include "../logic/elements/terrain.hpp"

typedef struct MapSize {
    int x;
    int y;
} MapSize;

typedef std::vector<Triangle*> TriangleMesh;

class NavMesh {
    public:
        // May throw InsufficientNodesException or FailedTriangulationException
        NavMesh(std::vector<Terrain*> terrains, MapSize map_size);
        MapSize getMapSize();
        TriangleMesh getMesh();
        std::vector<Node*> getNodes();

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
        std::vector<Node*> m_nodes;
        Triangle* legalize(Triangle* candidate, Hull* frontier);
        void triangulate(std::vector<Terrain*> terrains);
        void populateNodes();
        Coord avgCoord(std::vector<Node*> nodes);
};

#endif
