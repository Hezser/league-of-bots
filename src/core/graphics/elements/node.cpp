/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "node.hpp"
#include "edge.hpp"
#include "polygon.hpp"
#include <cmath>

using namespace adamant::graphics::elements;

Node::Node(Coord coord, Coord origin) {
    this->coord = coord;
    setOrigin(origin);
}

Node::Node(Coord coord, Coord origin, Edge* edge_ptr): Node(coord, origin) {
    edge_ptrs.push_back(edge_ptr);
}

Node::~Node() {
    for (Edge* e : edge_ptrs) {
        delete e;
    }
}

Edge* Node::getEdgeWith(Node* node) {
    for (auto e : edge_ptrs) {
        if (e->a == node || e->b == node) return e;
    }
    return nullptr;
}

void Node::setOrigin(Coord origin) {
    int polar_x = coord.x - origin.x;
    int polar_y = coord.y - origin.y;
    r = std::sqrt(std::pow(polar_x, 2) + std::pow(polar_y, 2));
    theta = std::atan2((double)polar_y, (double)polar_x);
    if (theta < 0) theta += 2 * M_PI;
}

bool Node::isOn(Edge* edge) {
    if(coord.x <= std::max(edge->a->coord.x, edge->b->coord.x) &&
       coord.x <= std::min(edge->a->coord.x, edge->b->coord.x) &&
      (coord.y <= std::max(edge->a->coord.y, edge->b->coord.y) &&
       coord.y <= std::min(edge->a->coord.y, edge->b->coord.y))) return true;
    return false;
}

bool Node::isRestrictedWith(Node* node) {
    for (Edge* e_ptr1 : edge_ptrs) {
        for (Edge* e_ptr2 : node->edge_ptrs) {
            for (Polygon* s_ptr1 : e_ptr1->shape_ptrs) {
                for (Polygon* s_ptr2 : e_ptr2->shape_ptrs) {
                    // They belong to the same shape
                    if (s_ptr1->type != Shape::triangle && s_ptr1 == s_ptr2) {
                        for (Edge* e_shape : s_ptr1->edges) {
                            if ((e_shape->a == this && e_shape->b == node) || 
                                (e_shape->a == node && e_shape->b == this)) goto cont;
                        }
                        // They also are not connected
                        return true;
                        cont:;
                    }
                }       
            }
        }
    }
    return false;
}

bool Node::RComparator::operator() (Node* lhs, Node* rhs) {
    if (lhs->r != rhs->r) {
        return lhs->r < rhs->r;
    }
    return rhs->theta < rhs->theta;
}

bool Node::ThetaComparator::operator() (Node* lhs, Node* rhs) {
    return lhs->theta <= rhs->theta;
}

