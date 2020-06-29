#ifndef NAV_MESH_HPP
#define NAV_MESH_HPP

#include <vector>
#include "../logic/elements/terrain.hpp"

typedef struct MapSize {
    int x;
    int y;
} MapSize;

typedef struct Node {
    int x;
    int y;
    float r;
    float theta;
    Node(int x, int y, Coord o);

    friend bool operator < (const Node& lhs, const Node& rhs);

    private:
        Node();
} Node;

typedef std::vector<Node> Edge;

typedef std::vector<Edge> Triangle;

typedef std::vector<Triangle> Mesh;

class NavMesh {
    public:
        NavMesh(std::vector<Terrain*> terrains, MapSize map_size);
        void addNode(Coord* coord);
        void addTerrain(Terrain* terrain);
        void updateMesh(std::vector<Terrain*> terrains);
        Mesh getMesh();
        MapSize getMapSize();
        std::vector<Node> getNodes();

    private:
        Mesh m_mesh;
        MapSize m_map_size;
        void triangulate(std::vector<Coord*> coords);
        Coord avgCoord(std::vector<Coord*> coords);
};

#endif
