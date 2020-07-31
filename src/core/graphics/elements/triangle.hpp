/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

#include "node.hpp"
#include "edge.hpp"
#include "convex_polygon.hpp"
#include <vector>

namespace adamant {
namespace graphics {
namespace elements {

class Triangle: public ConvexPolygon {
    public:
        Triangle(Node* a, Node* b, Node* c);
        Triangle(Edge* e, Node* n);
        Triangle(Edge* e, Edge* g);
        Node* nodeOppositeToEdge(Edge* edge);
        float angleOppositeToEdge(Edge* edge);
        std::vector<Edge*> adjacentEdges(Edge* edge);

        struct IllegalTriangleException: public std::exception {
            const char* what() const noexcept;
        };
    
    private:
        Triangle();
        bool areCollinear(Node* a, Node* b, Node* c);
};

}  // namespace elements
}  // namespace graphics
}  // namespace adamant

#endif
