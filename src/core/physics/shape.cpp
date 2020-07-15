#include "shape.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

/* struct Node */

Node::Node(Coord coord, Coord origin) {
    this->coord = coord;
    setOrigin(origin);
}

Node::Node(Coord coord, Coord origin, Edge* edge_ptr): Node(coord, origin) {
    edge_ptrs.push_back(edge_ptr);
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
    theta = std::atan(polar_y / polar_x);
}

float Node::shortestDistanceTo(Edge* edge) {
    return std::abs((edge->b->coord.y - edge->a->coord.y) * coord.x - 
                    (edge->b->coord.x - edge->a->coord.x) * coord.y +
                     edge->b->coord.x * edge->a->coord.y - edge->b->coord.y * edge->a->coord.x) /
           std::sqrt(std::pow(edge->b->coord.y - edge->a->coord.y, 2) +
                     std::pow(edge->b->coord.x - edge->a->coord.x, 2));
}

bool Node::isOn(Edge* edge) {
    if(coord.x <= std::max(edge->a->coord.x, edge->b->coord.x) &&
       coord.x <= std::min(edge->a->coord.x, edge->b->coord.x) &&
      (coord.y <= std::max(edge->a->coord.y, edge->b->coord.y) &&
       coord.y <= std::min(edge->a->coord.y, edge->b->coord.y))) return true;
    return false;
}

bool Node::RComparator::operator() (const Node& lhs, const Node& rhs) {
    if (lhs.r != rhs.r) {
        return lhs.r < rhs.r;
    }
    return rhs.theta < rhs.theta;
}

bool Node::ThetaComparator::operator() (const Node& lhs, const Node& rhs) {
    return lhs.theta <= rhs.theta;
}

/* struct Edge */

Edge::Edge(Node* a, Node* b, Shape* shape_ptr) {
    if (a->restricted.find(b) != a->restricted.end() ||
        b->restricted.find(a) != b->restricted.end()) throw IllegalEdgeException();
    this->a = a;
    this->b = b;
    a->edge_ptrs.push_back(this);
    b->edge_ptrs.push_back(this);
    this->length = std::sqrt(std::pow(std::abs(a->coord.x - b->coord.x), 2) + 
                             std::pow(std::abs(a->coord.y - b->coord.y), 2));
    this->shape_ptrs.push_back(shape_ptr);
}

Edge::Edge(Node* a, Node* b, Edge* left, Edge* right, Shape* shape_ptr):
        Edge(a, b, shape_ptr) {
    this->left = left;
    this->right = right;
}

Edge::~Edge() {
    // Remove edge from a's edge_ptrs
    auto pos = std::find(a->edge_ptrs.begin(), a->edge_ptrs.end(), this);
    if (pos != a->edge_ptrs.end()) a->edge_ptrs.erase(a->edge_ptrs.begin() + std::distance(a->edge_ptrs.begin(), pos));
    // Remove edge from b's edge_ptrs
    pos = std::find(b->edge_ptrs.begin(), b->edge_ptrs.end(), this);
    if (pos != b->edge_ptrs.end()) b->edge_ptrs.erase(b->edge_ptrs.begin() + std::distance(b->edge_ptrs.begin(), pos));
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

const char* Edge::IllegalEdgeException::what() const noexcept {
    return "The edge intercepts with terrain.";
}

/* struct Shape */

Shape::Shape(Coord center) {
    this->center = center;
}

/* struct Circle */

Circle::Circle(Coord center, int radius): Shape(center) {
    this->radius = radius;
}

/* struct Polygon */

Polygon::Polygon(std::vector<Node*> nodes, std::vector<Edge*> edges) {
    this->edges = edges;
    this->nodes = nodes;
    std::vector<Coord> coords;
    for (Edge* e : edges) {
        e->shape_ptrs.push_back(this);
    }
    for (Node* n : nodes) {
        coords.push_back(n->coord);
    }
    center = findCenter(coords);
    restrictNodes();
}

Polygon::~Polygon() {
    // Remove shape from each edge's shape_ptrs
    for (Edge* e : edges) {
        auto pos = std::find(e->shape_ptrs.begin(), e->shape_ptrs.end(), this);
        if (pos != e->shape_ptrs.end()) e->shape_ptrs.erase(e->shape_ptrs.begin() + std::distance(e->shape_ptrs.begin(), pos));
    }
}

Coord Polygon::findCenter(std::vector<Coord> coords) {
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

void Polygon::restrictNodes() {
    /* We even restrict those nodes that are connected to each other, because the edge
     * between them already exists and no other such edge should be created */
    for (auto n : nodes) {
        n->restricted.insert(nodes.begin(), nodes.end());
    }
}

/* struct ConvexPolygon */

ConvexPolygon::ConvexPolygon(std::vector<Coord> coords) {
    center = findCenter(coords);
    nodes = createNodes(coords);
    std::sort(nodes.begin(), nodes.end());
    for (auto i=0; i<nodes.size()-1; i++) {
        edges.emplace_back(nodes[i], nodes[i+1], this);
    }
    edges.emplace_back(nodes[nodes.size()-1], nodes[0], this);
    restrictNodes();
}

std::vector<Node*> ConvexPolygon::createNodes(std::vector<Coord> coords) {
    std::vector<Node*> nodes;
    for (Coord c : coords) {
        nodes.emplace_back(c.x, c.y, center);
    }
    return nodes;
}

/* struct Triangle */

/* If an edge already exists as part of another triangle, it is important to use it in the new
 * triangle constructor instead of creating an identical one, in order to keep triangle_ptrs 
 * updated */

// Used for the first triangle of the mesh
Triangle::Triangle(Node* a, Node* b, Node* c) {
    if (areCollinear(a, b, c)) throw IllegalTriangleException();
    center = {(a->coord.x + b->coord.x + c->coord.x) / 3, (a->coord.y + b->coord.y + c->coord.y) / 3};
    nodes = {a, b, c};
    edges = {new Edge(a, b, this), new Edge(b, c, this), new Edge(c, a, this)};
}

// Used for triangles created when adding a new node to the frontier
Triangle::Triangle(Edge* e, Node* n) {
    if (areCollinear(e->a, e->b, n)) throw IllegalTriangleException();
    center = {(e->a->coord.x + e->b->coord.x + n->coord.x) / 3, (e->a->coord.y + e->b->coord.y + n->coord.y) / 3};
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
    center = {(common_n->coord.x + diff_n[0]->coord.x + diff_n[1]->coord.x) / 3,
              (common_n->coord.y + diff_n[0]->coord.y + diff_n[1]->coord.y) / 3};
    nodes = {common_n, diff_n[0], diff_n[1]};
    edges = {e, g, new Edge(diff_n[0], diff_n[1], this)};
    e->shape_ptrs.push_back(this);
    g->shape_ptrs.push_back(this);
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

/* struct Hull */

Hull::Hull(Coord origin) {
    this->origin = origin;
}

Hull::Hull(Coord origin, std::vector<Edge*> edges) {
    this->origin = origin;
    this->edges = edges;
}

/* If there are >3 edges being collinear and a node intersects with them, the node needs to
 * get the closest edge to it (furtherst away from origin), which guarantees that the node can
 * make a non-collinear triangle with the left or right edge of the intersector */
Edge* Hull::popIntersectingEdge(Node* node) {
    // Empty hull
    if (edges.size() == 0) return nullptr;

    // The center of the hull is inside the hull itself
    int j = -1;
    for (int i=0; i<edges.size(); i++) {
        Edge* e = edges[i];
        /* The node's angle is within the edge's angles (it intersects with it)
         * AND the origin is not between the node and the intersector
         * AND the node is not between the origin and the intersector
         * AND the node is not one of the edge's nodes
         * AND the node is not collinear with the edge */
        if (((e->a->theta > node->theta && e->b->theta < node->theta) || 
             (e->b->theta > node->theta && e->a->theta < node->theta) ||
             (e->a->theta == node->theta)) && node->shortestDistanceTo(e) <= node->r &&
              node->r >= e->avgR() && e->a != node && e->b != node &&
              !e->isCollinearWithNode(node)) {
            // Pick the closest intersector to the node
            if (j == -1 || e->avgR() > edges[j]->avgR()) j = i;
        }
    }

    // If the closest intersector belongs to the same shape, we cannot use it
    if (j != -1 && belongToSameShape(node, edges[j])) j = -1;

    // We did not find a suitable intersector, so we choose the closest legal edge to the node
    std::vector<Edge*> es = edges;
    std::sort(es.begin(), es.end(), [node](Edge& lhs, Edge& rhs) {
        return node->shortestDistanceTo(&lhs) < node->shortestDistanceTo(&rhs);
    });
    std::vector<Edge*> closer;
    if (j == -1) {
        for (int i=0; i<es.size(); i++) {
            if (es[i]->a == node || edges[i]->b == node) continue;
            Edge* e;
            Edge* g;
            try {
                e = new Edge(node, es[i]->a, nullptr);
                g = new Edge(node, es[i]->b, nullptr);
            } catch (Edge::IllegalEdgeException) {
                closer.push_back(es[i]);
                continue;
            }
            // Check for legality
            bool legal = !es[i]->isCollinearWithNode(node) && 
                         !belongToSameShape(node, es[i]) && !e->intersectsWith(closer) &&
                         !g->intersectsWith(closer);
            delete e;
            delete g;
            if (legal) {
                auto pos = std::find(edges.begin(), edges.end(), es[i]);
                if (pos != edges.end()) j = std::distance(edges.begin(), pos);
                break;
            }
            closer.push_back(es[i]);
        }
    }

    if (j == -1) return nullptr; 

    Edge* intersector = edges[j];
    edges.erase(edges.begin() + j);
    return intersector;
}

bool Hull::belongToSameShape(Node* node, Edge* edge) {
    for (auto s : edge->shape_ptrs) {
        for (auto e : node->edge_ptrs) {
            for (auto t : e->shape_ptrs) {
                if (s == t) return true;
            }
        }
    }
    return false;
}
