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
        NavMesh(std::vector<Terrain*> terrains, MapSize map_size);
        MapSize getMapSize();
        TriangleMesh getMesh();
        std::vector<Node*> getNodes();

        struct InsufficientNodesException: public std::exception {
            const char* what() const throw();
        };

    private:
        MapSize m_map_size;
        TriangleMesh m_mesh;
        Hull* m_hull;
        std::vector<Node*> m_nodes;
        std::vector<Coord> calculateCoords(std::vector<Terrain*> terrains);
        void connectNode(Node* node);
        void legalize(Triangle* t);
        void triangulate(std::vector<Coord> coords);
        void populateNodes();
        Coord avgCoord(std::vector<Coord> coords);
};

#endif
