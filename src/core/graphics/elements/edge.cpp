/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "edge.hpp"
#include "polygon.hpp"
#include <algorithm>
#include <cmath>
#include <iostream> // delete

using namespace adamant::graphics::elements;

Edge::Edge(Node* a, Node* b) {
    Edge* existing_edge = a->getEdgeWith(b);
    if (existing_edge != nullptr) throw ExistingEdgeException(existing_edge);
    this->a = a;
    this->b = b;
    a->edge_ptrs.push_back(this);
    b->edge_ptrs.push_back(this);
    this->length = std::sqrt(std::pow(std::abs(a->coord.x - b->coord.x), 2) + 
                             std::pow(std::abs(a->coord.y - b->coord.y), 2));
    this->left = nullptr;
    this->right = nullptr;
}

Edge::Edge(Node* a, Node* b, Polygon* shape_ptr): Edge(a, b) {
    this->shape_ptrs.push_back(shape_ptr);
}

Edge::Edge(Node* a, Node* b, Edge* left, Edge* right, Polygon* shape_ptr):
        Edge(a, b, shape_ptr) {
    this->left = left;
    this->right = right;
}

/* Note that we do not delete its nodes, as nodes are meaningful even
 * if they do not belong to edges */
Edge::~Edge() {
    // Remove edge from a's edge_ptrs
    auto pos = std::find(a->edge_ptrs.begin(), a->edge_ptrs.end(), this);
    if (pos != a->edge_ptrs.end()) a->edge_ptrs.erase(pos);
    // Remove edge from b's edge_ptrs
    pos = std::find(b->edge_ptrs.begin(), b->edge_ptrs.end(), this);
    if (pos != b->edge_ptrs.end()) b->edge_ptrs.erase(pos);
    // Remove edge from shapes
    for (Polygon* s : shape_ptrs) {
        if (s != nullptr) {
            auto pos = std::find(s->edges.begin(), s->edges.end(), this);
            if (pos != s->edges.end()) s->edges.erase(pos);
        }
    }
    // Remove left/right references
    if (left != nullptr) left->right = nullptr;
    if (right != nullptr) right->left = nullptr;
}

/* This method has two edge cases, for which left/right is not defined:
 * 1. (a->theta - b->theta) == M_PI
 * 2. (a->theta - b->theta) == 0 (i.e: a->theta == b->theta) */
bool Edge::hasAtLeft(Edge* edge) {
    // Determine left-most node
    Node* left_node;
    if ((a->theta > b->theta && (a->theta - b->theta) < M_PI) ||
        (a->theta < b->theta && (b->theta - a->theta) > M_PI)) {
        left_node = a;
    } else {
        left_node = b;
    }
    // Determine if any node is the left node
    if (edge->a == left_node || edge->b == left_node) {
        return true;
    }
    return false;
}

double Edge::angleWith(Edge* edge) {
    Node* common_n = commonNodeWith(edge);
    if (common_n == nullptr || (left != edge && edge->left != this)) return -1;
    Node* left_n;
    Node* right_n;
    if (left == edge) {
        left_n = common_n == edge->a ? edge->b : edge->a;
        right_n = common_n == a ? b : a;
    } else {
        right_n = common_n == edge->a ? edge->b : edge->a;
        left_n = common_n == a ? b : a;
    }

    // Calculate angle
    double left_n_theta = std::atan2((double) left_n->coord.y - common_n->coord.y,
            (double) left_n->coord.x - common_n->coord.x);
    if (left_n_theta < 0) left_n_theta += 2 * M_PI;
    double right_n_theta = std::atan2((double) right_n->coord.y - common_n->coord.y,
            (double) right_n->coord.x - common_n->coord.x);
    if (right_n_theta < 0) right_n_theta += 2 * M_PI;
    double angle = std::abs(left_n_theta - right_n_theta);

    // Adjust angle to be the outer angle of the hull/polygon
    int det = (common_n->coord.x - right_n->coord.x) * (left_n->coord.y - common_n->coord.y) -
                (left_n->coord.x - common_n->coord.x) * (common_n->coord.y - right_n->coord.y);
    bool is_reflex = det > 0;
    if (is_reflex && angle < M_PI) angle += M_PI;
    else if (!is_reflex && angle > M_PI) angle -= M_PI;

    return angle;
}

float Edge::avgR() {
    return (a->r + b->r) / 2;
}

bool Edge::isCollinearWithNode(Node* node) {
    int area = a->coord.x * (b->coord.y - node->coord.y) +
               b->coord.x * (node->coord.y - a->coord.y) +
               node->coord.x * (a->coord.y - b->coord.y);
    if (area == 0) return true;
    return false;
}

bool Edge::intersectsWith(std::vector<Edge*> edges) {
    for (Edge* e : edges) {
        int dir1 = direction(a, b, e->a);
        int dir2 = direction(a, b, e->b);
        int dir3 = direction(e->a, e->b, a);
        int dir4 = direction(e->a, e->b, b);

        if(dir1 != dir2 && dir3 != dir4) return true;
        if(dir1==0 && e->a->isOn(this)) return true;
        if(dir2==0 && e->b->isOn(this)) return true;
        if(dir3==0 && a->isOn(e)) return true;
        if(dir4==0 && b->isOn(e)) return true;
    }
    return false;
}

float Edge::shortestDistanceTo(Coord coord) {
    return std::abs((b->coord.y - a->coord.y) * coord.x - 
                    (b->coord.x - a->coord.x) * coord.y +
                     b->coord.x * a->coord.y - b->coord.y * a->coord.x) /
           std::sqrt(std::pow(b->coord.y - a->coord.y, 2) +
                     std::pow(b->coord.x - a->coord.x, 2));
}

Node* Edge::commonNodeWith(Edge* edge) {
    if (a == edge->a || a == edge->b) return a;
    if (b == edge->a || b == edge->b) return b;
    return nullptr;
}

int Edge::direction(Node* a, Node* b, Node* c) {
    int val = (b->coord.y - a->coord.y) * (c->coord.x - b->coord.x) -
              (b->coord.x - a->coord.x) * (c->coord.y - b->coord.y);
    if (val == 0) return 0;      // Colinear
    else if (val < 0) return 2;  // Anti-clockwise direction
    return 1;                    // Clockwise direction
}

bool Edge::GreaterEdgeComparator::operator() (const Edge& lhs, const Edge& rhs) {
    return ((lhs.a->theta + rhs.b->theta) / 2) > ((rhs.a->theta + rhs.b->theta) / 2);
}

Edge::ExistingEdgeException::ExistingEdgeException(Edge* edge) {
    this->edge = edge;
}

const char* Edge::ExistingEdgeException::what() const noexcept {
    return "The edge already exists";
}

Edge* Edge::ExistingEdgeException::getExistingEdge() {
    return edge;
}

