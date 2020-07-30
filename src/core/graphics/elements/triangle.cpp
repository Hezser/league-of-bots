#include "triangle.hpp"
#include <cmath>

/* If an edge already exists as part of another triangle, it is important to use it in the new
 * triangle constructor instead of creating an identical one, in order to keep shape_ptrs 
 * updated */

// Used for the first triangle of the mesh
Triangle::Triangle(Node* a, Node* b, Node* c): ConvexPolygon(triangle) {
    if (areCollinear(a, b, c)) throw IllegalTriangleException();
    m_center = {(a->coord.x + b->coord.x + c->coord.x) / 3,
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
    m_center = {(e->a->coord.x + e->b->coord.x + n->coord.x) / 3,
              (e->a->coord.y + e->b->coord.y + n->coord.y) / 3};
    nodes = {e->a, e->b, n};
    Edge* g;
    Edge* h;
    try {
        g = new Edge(e->a, n, this);
    } catch (Edge::ExistingEdgeException &ex) {
        g = ex.getExistingEdge();
        g->shape_ptrs.push_back(this);
    }
    try {
        h = new Edge(e->b, n, this);
    } catch (Edge::ExistingEdgeException &ex) {
        h = ex.getExistingEdge();
        h->shape_ptrs.push_back(this);
    }
    edges = {e, g, h}; 
    e->shape_ptrs.push_back(this);
    constructDrawable({e->a->coord, e->b->coord, n->coord});
}

// Used for triangles created when left/right-side walking
Triangle::Triangle(Edge* e, Edge* g): ConvexPolygon(triangle) {
    Node* common_n = e->a == g->a || e->a == g->b ? e->a : e->b;
    std::vector<Node*> diff_n;
    if (e->a == common_n) diff_n.push_back(e->b);
    else diff_n.push_back(e->a);
    if (g->a == common_n) diff_n.push_back(g->b);
    else diff_n.push_back(g->a);
    if (areCollinear(common_n, diff_n[0], diff_n[1])) throw IllegalTriangleException();
    m_center = {(common_n->coord.x + diff_n[0]->coord.x + diff_n[1]->coord.x) / 3,
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

