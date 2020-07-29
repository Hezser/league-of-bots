#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <vector>
#include <exception>
#include <unordered_set>
#include <queue>
#include <SFML/Graphics.hpp>

// Foward-declaration
struct Polygon;
struct Edge;

typedef struct Coord {
    int x;
    int y;
} Coord;

class Node {
    public:
        Coord coord;
        float r;
        float theta;
        std::vector<Edge*> edge_ptrs;
        Node(Coord coord, Coord origin);
        Node(Coord coord, Coord origin, Edge* edge_ptr);
        ~Node();
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
};

class Edge {
    public:
        Node* a;
        Node* b;
        Edge* left;   // Triangles in meshes don't use this, because edges can be shared
        Edge* right;  // Triangles in meshes don't use this, because edges can be shared
        float length;
        std::vector<Polygon*> shape_ptrs;
        Edge(Node* a, Node* b);
        Edge(Node* a, Node* b, Polygon* shape_ptr);
        Edge(Node* a, Node* b, Edge* left, Edge* right, Polygon* shape_ptr);
        ~Edge();
        bool hasAtLeft(Edge* edge);
        float angleWith(Edge* edge);
        float avgR();
        bool isCollinearWithNode(Node* node);
        bool intersectsWith(std::vector<Edge*> edges);
        float shortestDistanceTo(Coord coord);
        Node* commonNodeWith(Edge* edge);

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

    private:
        int direction(Node* a, Node* b, Node* c);
        Edge();
};

class Shape {
    public:
        typedef enum ShapeType {
            circle = 0, polygon = 1, convex_polygon = 2, triangle = 3
        } ShapeType;

        ShapeType type;
        Coord getCenter();
        virtual void setCenter(Coord center);
        virtual sf::Shape* getDrawable() = 0;
        // TODO: Functions to modify drawable (color, outline, etc)
    
    protected:
        Shape(ShapeType type, Coord center);
        Shape(ShapeType type);  // Used for instantiating subclasses
        Coord m_center;
        sf::Shape* drawable;
};

/* TODO: Delete? 
 * Cannot be used for elem shapes, as circles do not have edges or nodes to triangulate,
 * check for collisions, etc */
class Circle: public Shape {
    public:
        int radius;
        Circle(Coord center, int radius);
        sf::Shape* getDrawable() override;

    private:
        Circle();
};

/* TODO: Polygons are not usable at the moment, as SFML does not support concave shapes
 *       However, there is no need for them now (DELETE?) */
class Polygon: public Shape {
    public:
        std::vector<Node*> nodes;
        std::vector<Edge*> edges;
        Polygon(std::vector<Node*> nodes, std::vector<Edge*> edges);
        ~Polygon();
        void setCenter(Coord center) override;
        void defineNeighboursFromCenter(Coord origin);
        sf::Shape* getDrawable() override;


        struct InsufficientNodesException: public std::exception {
            const char* what() const throw();
        };
    
    protected:
        Polygon(ShapeType subtype);  // Used for instantiating subclasses
        Coord findCenter(std::vector<Coord> coords);
};

class ConvexPolygon: public Polygon {
    public:
        ConvexPolygon(std::vector<Coord> coords);

    protected:
        ConvexPolygon(ShapeType subtype);  // Used for instantiating subclasses
        void constructDrawable(std::vector<Coord> coords);
        std::vector<Node*> createNodes(std::vector<Coord> coords);
};

class Triangle: public ConvexPolygon {
    public:
        Triangle(Node* a, Node* b, Node* c);
        Triangle(Edge* e, Node* n);
        Triangle(Edge* e, Edge* g);
        Node* nodeOppositeToEdge(Edge* edge);
        float angleOppositeToEdge(Edge* edge);
        std::vector<Edge*> adjacentEdges(Edge* edge);

        struct IllegalTriangleException {
            const char* what() const noexcept;
        };
    
    private:
        Triangle();
        bool areCollinear(Node* a, Node* b, Node* c);
};

class Hull {
    public:
        Coord origin;
        std::vector<Edge*> edges;
        Hull(Coord origin);
        Edge* getIntersectingEdge(Node* node);

    private:
        Hull();
} ;

#endif
