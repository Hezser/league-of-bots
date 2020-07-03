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

typedef struct Edge {
    Node* a;
    Node* b;
    float length;
    std::vector<Triangle*> triangle_ptrs;
    Edge(Node* a, Node* b, Triangle* triangle_ptr);
    void removeTrianglePtr(Triangle* triangle_ptr);
    bool hasAtLeft(Edge* edge);
    float angleWith(Edge* edge);

    struct GreaterEdgeComparator {
        bool operator() (const Edge& lhs, const Edge& rhs);
    };

    private:
        Edge();
} Edge;

// TODO: Implement it as a hash table of edge containers (contains: edge, right_edge, left_edge) to have O(1) access to edges and O(1) access to left/right edges
// class EdgePriorityQueue: public std::priority_queue<Edge*, std::vector<Edge*>, Edge::EdgeGreaterComparator> {
//     public:
//         int getIndexOf(Edge* edge);
//         Edge* getEdgeAt(int i);
// };

typedef struct HullEdgeContainer {
    Edge* edge;
    HullEdgeContainer* left;
    HullEdgeContainer* right;
    HullEdgeContainer(Edge* edge, HullEdgeContainer* left, HullEdgeContainer* right);
    HullEdgeContainer(Edge* edge);

    private:
        HullEdgeContainer();
} HullEdgeContainer;

typedef struct Hull {
    Coord o;
    // TODO: Implement it as a hash table of edge containers (contains: edge, right_edge, left_edge) to have O(1) access to edges and O(1) access to left/right edges
    std::vector<HullEdgeContainer*> edges;
    Hull(Coord o);
    Hull(Coord o, std::vector<HullEdgeContainer*> edges);
    HullEdgeContainer* popIntersectingEdge(Node* node);
    bool checkIntegrity();

    private:
        Hull();
} Hull;

typedef struct Triangle {
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    Triangle(Node* a, Node* b, Node* c);
    Triangle(Edge* e, Node* n);
    Node* nodeOppositeToEdge(Edge* edge);
    float angleOppositeToEdge(Edge* edge);
    std::vector<Edge*> adjacentEdges(Edge* edge);

    private:
        Triangle();
} Triangle;

typedef std::vector<Triangle*> TriangleMesh;

class NavMesh {
    public:
        NavMesh(std::vector<Terrain*> terrains, MapSize map_size);
        TriangleMesh getMesh();
        MapSize getMapSize();
        std::vector<Node> getNodes();

    private:
        TriangleMesh m_mesh;
        MapSize m_map_size;
        std::vector<Coord> calculateCoords(std::vector<Terrain*> terrains);
        void triangulate(std::vector<Coord> coords);
        void populateNodes();
        Coord avgCoord(std::vector<Coord> coords);
};

#endif
