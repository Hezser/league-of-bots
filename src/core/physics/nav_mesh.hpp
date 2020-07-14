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
        Hull* m_hull;
        std::vector<Node*> m_nodes;
        /* void connectNode(Node* node); */
        std::vector<Node*> getTerrainNodes(std::vector<Terrain*> terrains);
        Triangle* legalize(Triangle* candidate);
        void triangulate();
        void populateNodes();
        Coord avgCoord(std::vector<Node*> nodes);
};

#endif
