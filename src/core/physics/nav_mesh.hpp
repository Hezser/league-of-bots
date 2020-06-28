#ifndef NAV_MESH_HPP
#define NAV_MESH_HPP

#include <vector>
#include "../logic/elements/terrain.hpp"

typedef std::vector<int> Node;

typedef std::vector<Node> Edge;

typedef std::vector<Edge> Polygon;

typedef std::vector<Polygon> Mesh;

class NavMesh {
    public:
        NavMesh(std::vector<Terrain*> terrains);
        void addNode(Coord coord);
        void addTerrain(Terrain* terrain);
        void updateMesh(std::vector<Terrain*> terrains);
        Mesh getMesh();
        std::vector<Node> getNodes();

    private:
        Mesh m_mesh;
};

#endif
