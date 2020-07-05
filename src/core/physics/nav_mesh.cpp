#include "nav_mesh.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_set>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* struct Node */

Node::Node(int x, int y, Coord o) {
    this->x = x;
    this->y = y;
    int polar_x = x - o.x;
    int polar_y = y - o.y;
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

Edge::Edge(Node* a, Node* b, Triangle* triangle_ptr) {
    this->a = a;
    this->b = b;
    this->length = std::sqrt(std::pow(std::abs(a->x - b->x), 2) + 
                             std::pow(std::abs(a->y - b->y), 2));
    this->triangle_ptrs.push_back(triangle_ptr);
}

void Edge::removeTrianglePtr(Triangle* triangle_ptr) {
    auto pos = std::find(triangle_ptrs.begin(), triangle_ptrs.end(), triangle_ptr);
    if (pos != triangle_ptrs.end()) triangle_ptrs.erase(triangle_ptrs.begin() + std::distance(triangle_ptrs.begin(), pos));
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
    float m = (b->y - a->y) / (b->x - b->y);
    float edge_m = (edge->b->y - edge->a->y) / (edge->b->x - edge->b->y);
    float theta = std::atan(m);
    float edge_theta = std::atan(edge_m);
    return std::abs(theta - edge_theta);
}

bool Edge::GreaterEdgeComparator::operator() (const Edge& lhs, const Edge& rhs) {
    return ((lhs.a->theta + rhs.b->theta) / 2) > ((rhs.a->theta + rhs.b->theta) / 2);
}

/* struct HullEdgeContainer */

HullEdgeContainer::HullEdgeContainer(Edge* edge, HullEdgeContainer* left, HullEdgeContainer* right) {
    this->edge = edge;
    this->left = left;
    this->right = right;
}

HullEdgeContainer::HullEdgeContainer(Edge* edge) {
    this->edge = edge;
}

/* TODO: A hull may be more efficient to implement with nodes,
 * not edges (easier to find left/right nodes) */
/* struct Hull */

Hull::Hull(Coord o) {
    this->o = o;
}

Hull::Hull(Coord o, std::vector<HullEdgeContainer*> edges) {
    this->o = o;
    this->edges = edges;
}

HullEdgeContainer* Hull::popIntersectingEdge(Node* node) {
    for (int i=0; i<edges.size(); i++) {
        Edge* edge = edges[i]->edge;
        if ((edge->a->theta > node->theta && edge->b->theta < node->theta) || 
            (edge->b->theta > node->theta && edge->a->theta < node->theta) ||
            (edge->a->theta == node->theta)) {
            edges.erase(edges.begin() + i);
            return edges[i];
        }
    }
    // The node is within the Hull
    return nullptr;
}

// TODO: Can be improved now that we have left/right pointers
// To increase runtime speed, we do not check for integrity dynamically when adding edges
bool Hull::checkIntegrity() {
    Edge* e = edges[0]->edge;
    Node* start_n = e->a;
    Node* current_n = e->b;
    Edge* current_e = e;
    std::unordered_set<Node*> visited = {e->a, e->b};
    bool found;
    while (current_n != start_n) {
        found = false; 
        for (auto container : edges) {
            Edge* edge = container->edge;
            if (edge == current_e) continue;
            if (edge->a == current_n) {
                if (visited.find(edge->b) != visited.end()) return false;  // Inner cycles
                current_e = edge;
                current_n = edge->b;
                found = true;
                break;
            }
        }
        // Not closed
        if(!found) return false;
    }
    return true;
}

/* struct Triangle */

// TODO: At the creation of each triangle, check that the 3 points are not in line
Triangle::Triangle(Node* a, Node* b, Node* c) {
    nodes = {a, b, c};
    edges = {new Edge(a, b, this), new Edge(b, c, this), new Edge(c, a, this)};
}

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
    nodes = {common_n, diff_n[0], diff_n[1]};
    edges = {e, g, new Edge(diff_n[0], diff_n[1], this)};
}

Triangle::Triangle(Edge* e, Node* n) {
    nodes = {e->a, e->b, n};
    edges = {e, new Edge(e->a, n, this), new Edge(e->b, n, this)};
    e->triangle_ptrs.push_back(this);
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

/* class NavMesh */

NavMesh::NavMesh(std::vector<Terrain*> terrains, MapSize map_size): m_map_size{map_size} {
    triangulate(calculateCoords(terrains));
    populateNodes();
}

std::vector<Coord> NavMesh::calculateCoords(std::vector<Terrain*> terrains) {
    // TODO: implement
}

/* This is a custom implementation of:
 * A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
void NavMesh::triangulate(std::vector<Coord> coords) {
    if (coords.size() < 3) return;

    // INITIALIZATION
    m_mesh = TriangleMesh();
    Coord o = avgCoord(coords);
    std::vector<Node*> nodes;
    for (auto coord : coords) {
        nodes.emplace_back(coord.x, coord.y, o);
    }

    std::sort(nodes.begin(), nodes.end());
    int i = 0;

    // Special case where a node is at the origin (so that node[0]->r == 0)
    Node* o_node = nullptr;
    if (nodes[0]->x == o.x && nodes[0]->y == o.y) {  // Since node[0]->r is a float, let's not deal with accuracy
        o_node = nodes[0];
        nodes.erase(nodes.begin());
    }

    Triangle* t = new Triangle(nodes[0], nodes[1], nodes[2]);
    m_mesh.push_back(t);

    // Order the edges; left and right are considered looking from the origin of the hull
    HullEdgeContainer* anchor_e = new HullEdgeContainer(t->edges[0]);
    HullEdgeContainer* left_e = anchor_e->edge->hasAtLeft(t->edges[1]) ? new HullEdgeContainer(t->edges[1]) : new HullEdgeContainer(t->edges[2]);
    HullEdgeContainer* right_e = t->edges[1] == left_e->edge ? new HullEdgeContainer(t->edges[2]) : new HullEdgeContainer(t->edges[1]);
    anchor_e->left = left_e;
    anchor_e->right = right_e;
    left_e->left = right_e;
    left_e->right = anchor_e;
    right_e->left = anchor_e;
    right_e->right = left_e;
    Hull* frontier = new Hull(o, {anchor_e, left_e, right_e});

    i += 3;

    // TRIANGULATION
    while(i < nodes.size()) {
        // Create new triangle
        HullEdgeContainer* intersector_c = frontier->popIntersectingEdge(nodes[i]);
        Edge* intersector = intersector_c->edge;
        Triangle* candidate = new Triangle(intersector, nodes[i]);
        Triangle* neighbour = intersector->triangle_ptrs[0];

        // Legalize the triangle if it infringes the Delaunay condition
        float candidate_alpha = candidate->angleOppositeToEdge(intersector);
        float neighbour_alpha = neighbour->angleOppositeToEdge(intersector);
        if (candidate_alpha + neighbour_alpha > M_PI) {
            // Flip edges
            delete intersector;
            auto pos = std::find(m_mesh.begin(), m_mesh.end(), neighbour);
            if (pos != m_mesh.end()) m_mesh.erase(m_mesh.begin() + std::distance(m_mesh.begin(), pos));
            Node* candidate_a = candidate->nodeOppositeToEdge(intersector);
            std::vector<Edge*> neighbour_edges = neighbour->adjacentEdges(intersector);
            neighbour_edges[0]->removeTrianglePtr(neighbour);
            neighbour_edges[1]->removeTrianglePtr(neighbour);
            neighbour = new Triangle(neighbour_edges[0], candidate_a);
            candidate = new Triangle(neighbour_edges[1], candidate_a);
            m_mesh.push_back(neighbour);
        }

        // Update mesh and hull
        m_mesh.push_back(candidate);
        std::vector<Edge*> new_frontier = candidate->adjacentEdges(intersector);
        Edge* left_edge = new_frontier[0]->hasAtLeft(new_frontier[1]) ? new_frontier[1] : new_frontier[0];
        Edge* right_edge = new_frontier[0] == left_edge ? new_frontier[1] : new_frontier[0];
        HullEdgeContainer* left_edge_c = new HullEdgeContainer(left_edge);
        HullEdgeContainer* right_edge_c = new HullEdgeContainer(right_edge);
        left_edge_c->left = intersector_c->left;
        left_edge_c->right = right_edge_c;
        right_edge_c->left = left_edge_c;
        right_edge_c->right = intersector_c->right;
        frontier->edges.push_back(left_edge_c);
        frontier->edges.push_back(right_edge_c);

        // Left-side walk
        while(left_edge_c->edge->angleWith(left_edge_c->left->edge) < M_PI / 2) {
            Node* left_n = left_edge_c->left->edge->a == left_edge->a ||
                           left_edge_c->left->edge->a == left_edge->b ?
                           left_edge_c->left->edge->b : left_edge_c->left->edge->a;
            Triangle* t = new Triangle(left_edge, left_edge_c->left->edge, left_n);
            /* TODO: Check for Delaunay criteria with left_edge->triangle_ptrs[0]
             * and left_edge_c->left->edge->triangle_ptrs[0] */
            m_mesh.push_back(t);
        }

        // Right-side walk
        while(right_edge_c->edge->angleWith(right_edge_c->right->edge) < M_PI / 2) {
            // TODO: Make triangle
        }

        // TODO: Remove basins

        i++;
    }

    // FINALIZATION
    // Deal with special initialization case
    if (o_node != nullptr) {
        Triangle* t = m_mesh[0];
        m_mesh.erase(m_mesh.begin());
        m_mesh.emplace_back(t->edges[0], o_node);
        m_mesh.emplace_back(t->edges[1], o_node);
        m_mesh.emplace_back(t->edges[2], o_node);
    }
}

void NavMesh::populateNodes() {
    // TODO: implement
}

Coord NavMesh::avgCoord(std::vector<Coord> coords) {
    Coord max = {0, 0};  // Assuming no negative values, as coordinates are always positive
    Coord min = {m_map_size.x, m_map_size.y};
    for (auto coord : coords) {
        if (coord.x > max.x) max.x = coord.x;
        if (coord.x < min.x) min.x = coord.x;
        if (coord.y > max.y) max.y = coord.y;
        if (coord.y < min.y) min.y = coord.y;
    }
    return {(max.x + min.x) / 2, (max.y + min.y) / 2};
}

MapSize NavMesh::getMapSize() {
    return m_map_size;
}

TriangleMesh NavMesh::getMesh() {
    return m_mesh;
}

std::vector<Node*> NavMesh::getNodes() {
    return m_nodes;
}
