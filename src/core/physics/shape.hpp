#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <vector>
#include <exception>
#include <unordered_set>

// Foward-declaration
struct Shape;

typedef struct Coord {
    int x;
    int y;
} Coord;

typedef struct Node {
    Coord coord;
    float r;
    float theta;
    std::unordered_set<Node*> restricted;
    Node(Coord coord, Coord origin);

    friend bool operator < (const Node& lhs, const Node& rhs);

    private:
        Node();
} Node;

typedef struct Edge {
    Node* a;
    Node* b;
    Edge* left;  // Triangles in meshes don't use this, because edges can be shared
    Edge* right;  // Triangles in meshes don't use this, because edges can be shared
    float length;
    std::vector<Shape*> shape_ptrs;
    Edge(Node* a, Node* b, Shape* shape_ptr);
    Edge(Node* a, Node* b, Edge* left, Edge* right, Shape* shape_ptr);
    void removeShapePtr(Shape* shape_ptr);
    bool hasAtLeft(Edge* edge);
    float angleWith(Edge* edge);

    struct GreaterEdgeComparator {
        bool operator() (const Edge& lhs, const Edge& rhs);
    };

    struct IllegalEdgeException: public std::exception {
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

    struct IllegalTriangleException: public std::exception {
        const char* what() const noexcept;
    };
    
    private:
        Triangle();
        bool areCollinear(Node* a, Node* b, Node* c);
} Triangle;

#endif
