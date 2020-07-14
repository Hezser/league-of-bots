#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <vector>
#include <exception>
#include <unordered_set>
#include <queue>

// Foward-declaration
struct Shape;
struct Edge;

struct ShapeException: public std::exception {
    virtual const char* what() const noexcept;
};

typedef struct Coord {
    int x;
    int y;
} Coord;

typedef struct Node {
    Coord coord;
    float r;
    float theta;
    std::unordered_set<Node*> restricted;
    std::vector<Edge*> edge_ptrs;
    Node(Coord coord, Coord origin);
    Node(Coord coord, Coord origin, Edge* edge_ptr);
    Edge* getEdgeWith(Node* node);
    void setOrigin(Coord origin);
    float shortestDistanceTo(Edge* edge);

    struct RComparator {
        bool operator() (const Node& lhs, const Node& rhs);
    };

    struct ThetaComparator {
        bool operator() (const Node& lhs, const Node& rhs);
    };

    private:
        Node();
} Node;

typedef struct Edge {
    Node* a;
    Node* b;
    Edge* left;   // Triangles in meshes don't use this, because edges can be shared
    Edge* right;  // Triangles in meshes don't use this, because edges can be shared
    float length;
    std::vector<Shape*> shape_ptrs;
    Edge(Node* a, Node* b, Shape* shape_ptr);
    Edge(Node* a, Node* b, Edge* left, Edge* right, Shape* shape_ptr);
    ~Edge();
    bool hasAtLeft(Edge* edge);
    float angleWith(Edge* edge);
    float avgR();
    bool isCollinearWithNode(Node* node);

    struct GreaterEdgeComparator {
        bool operator() (const Edge& lhs, const Edge& rhs);
    };

    struct IllegalEdgeException: public ShapeException {
        const char* what() const noexcept;
    };

    private:
        Edge();
} Edge;

typedef struct Shape {
    Coord origin;
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    Shape(std::vector<Node*> nodes, std::vector<Edge*> edges);
    ~Shape();
    
    protected:
        Shape();
        Coord findOrigin(std::vector<Coord> coords);
        void restrictNodes();
} Shape;

typedef struct ConvexShape: Shape {
    ConvexShape(std::vector<Coord> coords);

    private:
        ConvexShape();
        std::vector<Node*> createNodes(std::vector<Coord> coords);
} ConvexShape;

typedef struct Triangle: Shape {
    Triangle(Node* a, Node* b, Node* c);
    Triangle(Edge* e, Node* n);
    Triangle(Edge* e, Edge* g, Node* n);
    Node* nodeOppositeToEdge(Edge* edge);
    float angleOppositeToEdge(Edge* edge);
    std::vector<Edge*> adjacentEdges(Edge* edge);

    struct IllegalTriangleException: public ShapeException {
        const char* what() const noexcept;
    };
    
    private:
        Triangle();
        bool areCollinear(Node* a, Node* b, Node* c);
} Triangle;

// TODO: Let hulls be just shapes or inherit from shapes (CAUTION WITH belongToSameShape())
typedef struct Hull {
    Coord origin;
    std::vector<Edge*> edges;
    Hull(Coord origin);
    Hull(Coord origin, std::vector<Edge*> edges);
    Edge* popIntersectingEdge(Node* node);

    private:
        bool belongToSameShape(Node* node, Edge* edge);
        Hull();
} Hull;

/* typedef struct Barrier { */
/*     void addNode(Node* node); */
/*     bool isClosed(); */

/*     private: */
/*         std::priority_queue<Node*, std::vector<Node*>, Node::ThetaComparator> m_nodes; */
/*         Node* m_min_theta_n; */
/*         Node* m_max_theta_n; */
/*         bool m_closed; */
/* } Barrier; */

#endif
