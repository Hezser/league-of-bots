#ifndef NAV_MESH_HPP
#define NAV_MESH_HPP

#include <vector>
#include "../logic/elements/terrain.hpp"

// Foward-declaration
struct Triangle;

typedef struct MapSize {
    int x;
    int y;
} MapSize;

typedef struct Node {
    Node(int x, int y, Coord o);
    int x;
    int y;
    float r;
    float theta;

    friend bool operator < (const Node& lhs, const Node& rhs);

    /* struct ThetaComparator { */
    /*     bool operator() (const Node& lhs, const Node& rhs); */
    /* }; */

    private:
        Node();
} Node;

/*
class NodePriorityQueue: public std::priority_queue<Node*, std::vector<Node*>, Node::ThetaComparator> {
    public:
        int getIndexOf(Node* node);
        Node* getNodeAt(int i);
};
*/

typedef struct Edge {
    Node* a;
    Node* b;
    float length;
    std::vector<Triangle*> triangle_ptrs;
    Edge(Node* a, Node* b, Triangle* triangle_ptr);
    void removeTrianglePtr(Triangle* triangle_ptr);

    private:
        Edge();
} Edge;

typedef struct Triangle {
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    // std::vector<Triangle*> neighbours;
    Triangle(Node* a, Node* b, Node* c);
    Triangle(Edge* e, Node* n);
    Node* nodeOpositeToEdge(Edge* edge);
    std::vector<Edge*> adjacentEdges(Edge* edge);

    private:
        Triangle();
} Triangle;

typedef struct Hull {
    Coord o;
    std::vector<Edge*> edges;
    Hull(Coord o);
    Hull(Coord o, std::vector<Edge*> edges);
    Edge* popIntersectingEdge(Node* node);
    bool checkIntegrity();

    private:
        Hull();
} Hull;


typedef std::vector<Triangle*> TriangleMesh;

class NavMesh {
    public:
        NavMesh(std::vector<Terrain*> terrains, MapSize map_size);
        void addNode(Coord* coord);
        void addTerrain(Terrain* terrain);
        void updateMesh(std::vector<Terrain*> terrains);
        TriangleMesh getMesh();
        MapSize getMapSize();
        std::vector<Node> getNodes();

    private:
        TriangleMesh m_mesh;
        MapSize m_map_size;
        void triangulate(std::vector<Coord*> coords);
        Coord avgCoord(std::vector<Coord*> coords);
};

#endif
