#include "shape.hpp"
#include <cmath>
#include <iostream>
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

/* struct Edge */

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
    std::cout << "\n DELETING EDGE " << this << "\n\n";
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

float Edge::angleWith(Edge* edge) {
    Node* common_n = a == edge->a || a == edge->b ? a : b;
    if (((common_n != a && common_n != b)) || (common_n != edge->a && common_n != edge->b)) {
        return -1;
    }
    Node* a_n = a == common_n ? b : a;
    Node* b_n = edge->a == common_n ? edge->b : edge->a;
    float a_n_theta = std::atan2((double) a_n->coord.y - common_n->coord.y,
            (double) a_n->coord.x - common_n->coord.x);
    if (a_n_theta < 0) a_n_theta += 2 * M_PI;
    float b_n_theta = std::atan2((double) b_n->coord.y - common_n->coord.y,
            (double) b_n->coord.x - common_n->coord.x);
    if (b_n_theta < 0) b_n_theta += 2 * M_PI;
    float angle = std::abs(b_n_theta - a_n_theta);
    bool is_reflex = a_n->getEdgeWith(b_n) != nullptr;
    if (is_reflex && angle < M_PI) angle += M_PI;
    return std::abs(angle);
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

/* struct Shape */

Shape::Shape(ShapeType type) {
    this->type = type;
}

Shape::Shape(ShapeType type, Coord center): Shape(type) {
    this->center = center;
}

sf::Shape* Shape::getDrawable() {
    drawable->setPosition(center.x, center.y);
    return drawable;
}

/* struct Circle */

Circle::Circle(Coord center, int radius): Shape(circle, center) {
    this->radius = radius;
    sf::CircleShape* d = new sf::CircleShape();
    d->setRadius(radius);
    this->drawable = d;
}

/* struct Polygon */

Polygon::Polygon(ShapeType subtype): Shape(subtype) {}

Polygon::Polygon(std::vector<Node*> nodes, std::vector<Edge*> edges): Shape(polygon) {
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
    // TODO: SFML does not supprot concave shapes, so we can't create the drawable
}

Polygon::~Polygon() {
    for (Edge* e : edges) {
        if (e->shape_ptrs.size() == 1) {
            // Delete the edge if it only belongs to this shape
            delete e;
        } else {
            // Remove shape from each edge's shape_ptrs otherwise
            auto pos = std::find(e->shape_ptrs.begin(), e->shape_ptrs.end(), this);
            if (pos != e->shape_ptrs.end()) e->shape_ptrs.erase(pos);
        }
    }
}

Coord Polygon::findCenter(std::vector<Coord> coords) {
    if (coords.size() == 0) return {0,0};
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

void Polygon::defineNeighboursFromCenter(Coord origin) {
    // Calculate r and theta with respect to the center
    for (Node* n : nodes) {
        n->setOrigin(center);
    }

    // Define left & right
    for (Edge* e : edges) {
        for (Edge* g : edges) {
            if (e != g) {
                if (e->hasAtLeft(g)) {
                    e->left = g;
                    g->right = e;
                } else {
                    e->right = g;
                    g->left = e;
                }
            }
        }
    }

    // Restore original center
    for (Node* n : nodes) {
        n->setOrigin(origin);
    }
}

const char* Polygon::InsufficientNodesException::what() const throw() {
    return "Less than 3 nodes were given.";
}

/* struct ConvexPolygon */

ConvexPolygon::ConvexPolygon(ShapeType subtype): Polygon(subtype) {}

ConvexPolygon::ConvexPolygon(std::vector<Coord> coords): Polygon(convex_polygon) {
    if (coords.size() < 3) throw InsufficientNodesException();

    center = findCenter(coords);

    // Create nodes
    nodes = createNodes(coords);
    std::sort(nodes.begin(), nodes.end(), Node::ThetaComparator());

    // Create edges
    for (int i=0; i<(int)nodes.size()-1; i++) {
        Edge* e;
        try {
            e = new Edge(nodes[i], nodes[i+1], this);
        } catch (Edge::ExistingEdgeException &ex) {
            e = ex.getExistingEdge();
            e->shape_ptrs.push_back(this);
        }
        edges.push_back(e);
    }
    if (nodes.size() > 1) {
        Edge* e;
        try {
            e = new Edge(nodes[nodes.size()-1], nodes[0], this);
        } catch (Edge::ExistingEdgeException &ex) {
            e = ex.getExistingEdge();
            e->shape_ptrs.push_back(this);
        }
        edges.push_back(e);
    }

    // Set edges' left and right
    for (int i=1; i<(int)edges.size()-1; i++) {
        edges[i]->left = edges[i+1];
        edges[i]->right = edges[i-1];
    }
    edges[0]->left = edges[1];
    edges[0]->right = edges[(int)edges.size()-1];
    edges[(int)edges.size()-1]->left = edges[0];
    edges[(int)edges.size()-1]->right = edges[(int)edges.size()-2];

    constructDrawable(coords);
}

void ConvexPolygon::constructDrawable(std::vector<Coord> coords) {
    sf::ConvexShape* d = new sf::ConvexShape();
    d->setPointCount(coords.size());
    for (auto i=0; i<coords.size(); i++) {
        d->setPoint(i, sf::Vector2f(coords[i].x, coords[i].y));
    }
    drawable = d;
}

std::vector<Node*> ConvexPolygon::createNodes(std::vector<Coord> coords) {
    std::vector<Node*> nodes;
    for (Coord c : coords) {
        nodes.push_back(new Node({c.x, c.y}, center));
    }
    return nodes;
}

/* struct Triangle */

/* If an edge already exists as part of another triangle, it is important to use it in the new
 * triangle constructor instead of creating an identical one, in order to keep shape_ptrs 
 * updated */

// Used for the first triangle of the mesh
Triangle::Triangle(Node* a, Node* b, Node* c): ConvexPolygon(triangle) {
    if (areCollinear(a, b, c)) throw IllegalTriangleException();
    center = {(a->coord.x + b->coord.x + c->coord.x) / 3,
              (a->coord.y + b->coord.y + c->coord.y) / 3};
    nodes = {a, b, c};
    Edge* e;
    Edge* g;
    Edge* h;
    try {
        e = new Edge(a, b, this);
    } catch (Edge::ExistingEdgeException &ex) {
        e = ex.getExistingEdge();
        e->shape_ptrs.push_back(this);
    }
    try {
        g = new Edge(b, c, this);
    } catch (Edge::ExistingEdgeException &ex) {
        g = ex.getExistingEdge();
        g->shape_ptrs.push_back(this);
    }
    try {
        h = new Edge(c, a, this);
    } catch (Edge::ExistingEdgeException &ex) {
        h = ex.getExistingEdge();
        h->shape_ptrs.push_back(this);
    }
    edges = {e, g, h};
    constructDrawable({a->coord, b->coord, c->coord});
}

// Used for triangles created when adding a new node to the frontier
Triangle::Triangle(Edge* e, Node* n): ConvexPolygon(triangle) {
    if (areCollinear(e->a, e->b, n)) throw IllegalTriangleException();
    center = {(e->a->coord.x + e->b->coord.x + n->coord.x) / 3,
              (e->a->coord.y + e->b->coord.y + n->coord.y) / 3};
    nodes = {e->a, e->b, n};
    Edge* g;
    Edge* h;
    std::cout << "Creating TRIANGLE(" << e << ", " << n << ")\n";
    std::cout << "\te->a = " << e->a << "\n";
    std::cout << "\te->b = " << e->b << "\n";
    std::cout << "\tn = " << n << "\n";
    try {
        g = new Edge(e->a, n, this);
    } catch (Edge::ExistingEdgeException &ex) {
        std::cout << "\tG EXISTS!\n";
        g = ex.getExistingEdge();
        g->shape_ptrs.push_back(this);
    }
    try {
        h = new Edge(e->b, n, this);
    } catch (Edge::ExistingEdgeException &ex) {
        std::cout << "\tH EXISTS!\n";
        h = ex.getExistingEdge();
        h->shape_ptrs.push_back(this);
    }
    std::cout << "\tg = " << g << "\n";
    std::cout << "\t\tg->a = " << g->a << "\n";
    std::cout << "\t\tg->b = " << g->b << "\n";
    std::cout << "\th = " << h << "\n";
    std::cout << "\t\th->a = " << h->a << "\n";
    std::cout << "\t\th->b = " << h->b << "\n";
    edges = {e, g, h}; 
    e->shape_ptrs.push_back(this);
    constructDrawable({e->a->coord, e->b->coord, n->coord});
}

// Used for triangles created when left/right-side walking
Triangle::Triangle(Edge* e, Edge* g): ConvexPolygon(triangle) {
    Node* common_n = e->a == g->a || e->a == g->b ? e->a : e->b;
    std::cout << "----- e = " << e << "\n";
    std::cout << "\t----- e->a = " << e->a << "\n";
    std::cout << "\t----- e->b = " << e->b << "\n";
    std::cout << "----- g = " << g << "\n";
    std::cout << "\t----- g->a = " << g->a << "\n";
    std::cout << "\t----- g->b = " << g->b << "\n";
    std::cout << "----- commmon node = " << common_n << "\n";
    std::vector<Node*> diff_n;
    if (e->a == common_n) diff_n.push_back(e->b);
    else diff_n.push_back(e->a);
    if (g->a == common_n) diff_n.push_back(g->b);
    else diff_n.push_back(g->a);
    if (areCollinear(common_n, diff_n[0], diff_n[1])) throw IllegalTriangleException();
    center = {(common_n->coord.x + diff_n[0]->coord.x + diff_n[1]->coord.x) / 3,
              (common_n->coord.y + diff_n[0]->coord.y + diff_n[1]->coord.y) / 3};
    nodes = {common_n, diff_n[0], diff_n[1]};
    Edge* h;
    try {
        h = new Edge(diff_n[0], diff_n[1], this);
    } catch (Edge::ExistingEdgeException &ex) {
        h = ex.getExistingEdge();
        h->shape_ptrs.push_back(this);
    }
    edges = {e, g, h};
    e->shape_ptrs.push_back(this);
    g->shape_ptrs.push_back(this);
    constructDrawable({common_n->coord, diff_n[0]->coord, diff_n[1]->coord});
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

/* If there are >3 edges being collinear and a node intersects with them, the node needs to
 * get the closest edge to it (furtherst away from origin), which guarantees that the node can
 * make a non-collinear triangle with the left or right edge of the intersector */
Edge* Hull::getIntersectingEdge(Node* node) {
    // Empty hull
    if (edges.size() == 0) return nullptr;

    // The center of the hull is inside the hull itself
    int j = -1;
    for (int i=0; i<edges.size(); i++) {
        Edge* e = edges[i];
        /* The node's angle is within the edge's angles (it intersects with it),
         * accounting for the exception of when e passes through theta=0 */
        if ((e->a->theta > node->theta && e->b->theta < node->theta) || 
             (e->b->theta > node->theta && e->a->theta < node->theta) ||
             (e->a->theta > node->theta && e->b->theta > node->theta && 
                    std::abs(e->a->theta - e->b->theta) >= M_PI) ||
             (e->a->theta == node->theta)) {
            float e_to_node = e->shortestDistanceTo(node->coord);
            // Pick the closest intersector to the node
            if (j == -1 || e->shortestDistanceTo(node->coord) <
                edges[j]->shortestDistanceTo(node->coord)) j = i;
        }
    }

    // We did not find a suitable intersector, so we choose the closest legal edge to the node
    if (j == -1) {
        std::vector<Edge*> es = edges;
        std::sort(es.begin(), es.end(), [&](Edge* lhs, Edge* rhs) {
            return lhs->shortestDistanceTo(node->coord) < rhs->shortestDistanceTo(node->coord);
        });
        std::vector<Edge*> closer;
        for (int i=0; i<es.size(); i++) {
            if (es[i]->a == node || es[i]->b == node) continue;
            Edge* e;
            Edge* g;
            bool e_exists = false;
            bool g_exists = false;
            try {
                e = new Edge(node, es[i]->a);
            } catch (Edge::ExistingEdgeException &ex) {
                e = ex.getExistingEdge();
                e_exists = true;
            }
            try {
                g = new Edge(node, es[i]->b);
            } catch (Edge::ExistingEdgeException &ex) {
                g = ex.getExistingEdge();
                g_exists = true;
            }
            // Check for legality
            bool legal = !es[i]->isCollinearWithNode(node) && !e->intersectsWith(closer) &&
                         !g->intersectsWith(closer);
            if (!e_exists) delete e;
            if (!g_exists) delete g;
            if (legal) {
                auto pos = std::find(edges.begin(), edges.end(), es[i]);
                if (pos != edges.end()) j = std::distance(edges.begin(), pos);
                break;
            }
            closer.push_back(es[i]);
        }
    }

    if (j == -1) return nullptr; 

    return edges[j];
}
