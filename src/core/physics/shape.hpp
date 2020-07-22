#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <vector>
#include <exception>
#include <unordered_set>
#include <queue>
#include <SFML/Graphics.hpp>

// Foward-declaration
struct Shape;
struct Edge;

struct IllegalShapeException: public std::exception {
    virtual const char* what() const throw() = 0;
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
    bool isOn(Edge* edge);

    struct RComparator {
        bool operator() (Node* lhs, Node* rhs);
    };

    struct ThetaComparator {
        bool operator() (Node* lhs, Node* rhs);
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
    bool intersectsWith(std::vector<Edge*> edges);
    float shortestDistanceTo(Coord coord);

    struct GreaterEdgeComparator {
        bool operator() (const Edge& lhs, const Edge& rhs);
    };

    struct ExistingEdgeException {
        Edge* edge;
        ExistingEdgeException(Edge* edge);
        const char* what() const noexcept;
        Edge* getExistingEdge();
        private:
            ExistingEdgeException();
    };

    struct IllegalEdgeException: public IllegalShapeException {
        const char* what() const noexcept;
    };

    private:
        int direction(Node* a, Node* b, Node* c);
        Edge();
} Edge;

typedef struct Shape {
    Coord center;
    sf::Shape* getDrawable();
    // TODO: Functions to modify drawable (color, outline, etc)
    
    protected:
        Shape(Coord center);
        Shape();
        sf::Shape* drawable;
} Shape;

/* TODO: Delete? 
 * Cannot be used for elem shapes, as circles do not have edges or nodes to triangulate,
 * check for collisions, etc */
typedef struct Circle: Shape {
    int radius;
    Circle(Coord center, int radius);

    private:
        Circle();
} Circle;

/* TODO: Polygons are not usable at the moment, as SFML does not support concave shapes
 *       However, there is no need for them now */
typedef struct Polygon: Shape {
    std::vector<Node*> nodes;
    std::vector<Edge*> edges;
    Polygon(std::vector<Node*> nodes, std::vector<Edge*> edges);
    ~Polygon();
    
    protected:
        Polygon();
        Coord findCenter(std::vector<Coord> coords);
        void restrictNodes();
} Polygon;

typedef struct ConvexPolygon: Polygon {
    ConvexPolygon(std::vector<Coord> coords);

    protected:
        ConvexPolygon();
        void constructDrawable(std::vector<Coord> coords);
        std::vector<Node*> createNodes(std::vector<Coord> coords);
} ConvexPolygon;

typedef struct Triangle: ConvexPolygon {
    Triangle(Node* a, Node* b, Node* c);
    Triangle(Edge* e, Node* n);
    Triangle(Edge* e, Edge* g, Node* n);
    Node* nodeOppositeToEdge(Edge* edge);
    float angleOppositeToEdge(Edge* edge);
    std::vector<Edge*> adjacentEdges(Edge* edge);

    struct IllegalTriangleException: public IllegalShapeException {
        const char* what() const noexcept;
    };
    
    private:
        Triangle();
        bool areCollinear(Node* a, Node* b, Node* c);
} Triangle;

typedef struct Hull {
    Coord origin;
    std::vector<Edge*> edges;
    Hull(Coord origin);
    Edge* popIntersectingEdge(Node* node);

    private:
        bool belongToSameShape(Node* node, Edge* edge);
        Hull();
} Hull;

#endif
