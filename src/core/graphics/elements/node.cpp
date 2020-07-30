#include "node.hpp"
#include "edge.hpp"
#include <cmath>

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

bool Node::RComparator::operator() (Node* lhs, Node* rhs) {
    if (lhs->r != rhs->r) {
        return lhs->r < rhs->r;
    }
    return rhs->theta < rhs->theta;
}

bool Node::ThetaComparator::operator() (Node* lhs, Node* rhs) {
    return lhs->theta <= rhs->theta;
}

