/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef EDGE_HPP
#define EDGE_HPP

#include "../coord.hpp"
#include "node.hpp"
#include <vector>

namespace adamant {
namespace graphics {
namespace elements {

// Forward declaration
class Polygon;

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
        double angleWith(Edge* edge);
        float avgR();
        bool isCollinearWithNode(Node* node);
        bool intersectsWith(std::vector<Edge*> edges);
        float shortestDistanceTo(Coord coord);
        Node* commonNodeWith(Edge* edge);

        struct GreaterEdgeComparator {
            bool operator() (const Edge& lhs, const Edge& rhs);
        };

        struct ExistingEdgeException: public std::exception {
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

}  // namespace elements
}  // namespace graphics
}  // namespace adamant

#endif
