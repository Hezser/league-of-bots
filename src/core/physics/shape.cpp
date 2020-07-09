#include "shape.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

/* struct Node */

Node::Node(Coord coord, Coord origin) {
    this->coord = coord;
    int polar_x = coord.x - origin.x;
    int polar_y = coord.y - origin.y;
    this->r = std::sqrt(std::pow(polar_x, 2) + std::pow(polar_y, 2));
    this->theta = std::atan(polar_y / polar_x);
}

bool operator < (const Node& lhs, const Node& rhs) {
    if (lhs.r != rhs.r) {
        return lhs.r < rhs.r;
    }
    return rhs.theta < rhs.theta;
}

/* struct Edge */

Edge::Edge(Node* a, Node* b, Shape* shape_ptr) {
    if (a->restricted.find(b) != a->restricted.end() ||
        b->restricted.find(a) != b->restricted.end()) throw IllegalEdgeException();
    this->a = a;
    this->b = b;
    this->length = std::sqrt(std::pow(std::abs(a->coord.x - b->coord.x), 2) + 
                             std::pow(std::abs(a->coord.y - b->coord.y), 2));
    this->shape_ptrs.push_back(shape_ptr);
}

Edge::Edge(Node* a, Node* b, Edge* left, Edge* right, Shape* shape_ptr):
        Edge(a, b, shape_ptr) {
    this->left = left;
    this->right = right;
}

void Edge::removeShapePtr(Shape* shape_ptr) {
    auto pos = std::find(shape_ptrs.begin(), shape_ptrs.end(), shape_ptr);
    if (pos != shape_ptrs.end()) shape_ptrs.erase(shape_ptrs.begin() + std::distance(shape_ptrs.begin(), pos));
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

float Edge::angleWith(Edge* edge) {
    float m = (b->coord.y - a->coord.y) / (b->coord.x - a->coord.x);
    float edge_m = (edge->b->coord.y - edge->a->coord.y) / (edge->b->coord.x - edge->b->coord.y);
    float theta = std::atan(m);
    float edge_theta = std::atan(edge_m);
    return std::abs(theta - edge_theta);
}

float Edge::avgR() {
    return (a->r + b->r) / 2;
}

bool Edge::GreaterEdgeComparator::operator() (const Edge& lhs, const Edge& rhs) {
    return ((lhs.a->theta + rhs.b->theta) / 2) > ((rhs.a->theta + rhs.b->theta) / 2);
}

const char* Edge::IllegalEdgeException::what() const noexcept {
    return "The edge intercepts with terrain.";
}

/* struct Shape */

Shape::Shape(std::vector<Node*> nodes, std::vector<Edge*> edges) {
    this->edges = edges;
    this->nodes = nodes;
    std::vector<Coord> coords;
    for (Node* n : nodes) {
        coords.push_back(n->coord);
    }
    origin = findOrigin(coords);
    restrictNodes();
}

Coord Shape::findOrigin(std::vector<Coord> coords) {
    Coord max = {0, 0};  // Assuming no negative values, as coordinates are always positive
    Coord min = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    for (auto coord : coords) {
        if (coord.x > max.x) max.x = coord.x;
        if (coord.x < min.x) min.x = coord.x;
        if (coord.y > max.y) max.y = coord.y;
        if (coord.y < min.y) min.y = coord.y;
    }
    return {(max.x + min.x) / 2, (max.y + min.y) / 2};
}

void Shape::restrictNodes() {
    for (auto n : nodes) {
        n->restricted.insert(nodes.begin(), nodes.end());
    }
    for (auto e : edges) {
        // Remove b from a's restricted
        e->a->restricted.erase(e->b);
        // Remove a from b's restricted
        e->b->restricted.erase(e->a);
    }
}

/* struct ConvexShape */

ConvexShape::ConvexShape(std::vector<Coord> coords) {
    origin = findOrigin(coords);
    nodes = createNodes(coords);
    std::sort(nodes.begin(), nodes.end());
    for (auto i=0; i<nodes.size()-1; i++) {
        edges.emplace_back(nodes[i], nodes[i+1]);
    }
    edges.emplace_back(nodes[nodes.size()-1], nodes[0]);
    restrictNodes();
}

std::vector<Node*> ConvexShape::createNodes(std::vector<Coord> coords) {
    std::vector<Node*> nodes;
    for (Coord c : coords) {
        nodes.emplace_back(c.x, c.y, origin);
    }
    return nodes;
}

/* struct Triangle */

// TODO: Throw IllegalTriangleException when appropiate

/* If an edge already exists as part of another triangle, it is important to use it in the new
 * triangle constructor instead of creating an identical one, in order to keep triangle_ptrs 
 * updated */

// Used for the first triangle of the mesh
Triangle::Triangle(Node* a, Node* b, Node* c) {
    if (areCollinear(a, b, c)) throw IllegalTriangleException();
    origin = {(a->coord.x + b->coord.x + c->coord.x) / 3, (a->coord.y + b->coord.y + c->coord.y) / 3};
    nodes = {a, b, c};
    edges = {new Edge(a, b, this), new Edge(b, c, this), new Edge(c, a, this)};
}

// Used for traingles created when adding a new node to the frontier
Triangle::Triangle(Edge* e, Node* n) {
    if (areCollinear(e->a, e->b, n)) throw IllegalTriangleException();
    origin = {(e->a->coord.x + e->b->coord.x + n->coord.x) / 3, (e->a->coord.y + e->b->coord.y + n->coord.y) / 3};
    nodes = {e->a, e->b, n};
    edges = {e, new Edge(e->a, n, this), new Edge(e->b, n, this)};
    e->shape_ptrs.push_back(this);
}

// Used for triangles created when left/right-side walking
Triangle::Triangle(Edge* e, Edge* g, Node* n) {
    Node* common_n = e->a == g->a || e->a == g->b ? e->a : e->b;
    std::vector<Node*> diff_n;
    if (e->a == common_n) {
        diff_n.push_back(e->b);
    } else {
        diff_n.push_back(e->a);
    }
    if (g->a == common_n) {
        diff_n.push_back(g->b);
    } else {
        diff_n.push_back(g->a);
    }
    if (areCollinear(common_n, diff_n[0], diff_n[1])) throw IllegalTriangleException();
    origin = {(common_n->coord.x + diff_n[0]->coord.x + diff_n[1]->coord.x) / 3,
              (common_n->coord.y + diff_n[0]->coord.y + diff_n[1]->coord.y) / 3};
    nodes = {common_n, diff_n[0], diff_n[1]};
    edges = {e, g, new Edge(diff_n[0], diff_n[1], this)};
}

Node* Triangle::nodeOppositeToEdge(Edge* edge) {
    for (auto node : nodes) {
        if (node != edge->a && node != edge->b) {
            return node;
        }
    }
    return nullptr;
}

float Triangle::angleOppositeToEdge(Edge* edge) {
    Node* a = nodeOppositeToEdge(edge);
    std::vector<Edge*> adj_edges = adjacentEdges(edge);
    float alpha = std::acos((std::pow(adj_edges[0]->length, 2) + std::pow(adj_edges[1]->length, 2) - std::pow(edge->length, 2)) / (2 * adj_edges[0]->length * adj_edges[1]->length));
    return alpha;
}


std::vector<Edge*> Triangle::adjacentEdges(Edge* edge) {
    std::vector<Edge*> adjacent;
    for (auto e : edges) {
        if (e == edge) continue;
        adjacent.push_back(e);
    }
    return adjacent;
}

const char* Triangle::IllegalTriangleException::what() const noexcept {
    return "A triangle's nodes are collinear.";
}

bool Triangle::areCollinear(Node* a, Node* b, Node* c) {
    int area = a->coord.x * (b->coord.y - c->coord.y) +
               b->coord.x * (c->coord.y - a->coord.y) +
               c->coord.x * (a->coord.y - b->coord.y);
    if (area == 0) return true;
    return false;
}
