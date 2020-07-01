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

/* bool Node::ThetaComparator::operator() (const Node& lhs, const Node& rhs) { */
/*     return lhs.theta >= rhs.theta; */
/* } */

/* class NodePriorityQueue */

/*
int NodePriorityQueue::getIndexOf(Node* node) {
    
}
*/

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

/* struct Triangle */

// TODO: At the creation of each triangle, check that the 3 points are not in line
Triangle::Triangle(Node* a, Node* b, Node* c) {
    nodes = {a, b, c};
    edges = {new Edge(a, b, this), new Edge(b, c, this), new Edge(c, a, this)};
}

Triangle::Triangle(Edge* e, Node* n) {
    nodes = {e->a, e->b, n};
    edges = {e, new Edge(e->a, n, this), new Edge(e->b, n, this)};
    e->triangle_ptrs.push_back(this);
}

Node* Triangle::nodeOpositeToEdge(Edge* edge) {
    for (auto node : nodes) {
        if (node != edge->a && node != edge->b) {
            return node;
        }
    }
    return nullptr;
}


std::vector<Edge*> Triangle::adjacentEdges(Edge* edge) {
    std::vector<Edge*> adjacent;
    for (auto e : edges) {
        if (e == edge) continue;
        adjacent.push_back(e);
    }
    return adjacent;
}

/* struct Hull */

Hull::Hull(Coord o) {
    this->o = o;
}

Hull::Hull(Coord o, std::vector<Edge*> edges) {
    this->o = o;
    this->edges = edges;
}

Edge* Hull::popIntersectingEdge(Node* node) {
    for (int i=0; i<edges.size(); i++) {
        Edge* edge = edges[i];
        if ((edge->a->theta > node->theta && edge->b->theta < node->theta) || 
            (edge->b->theta > node->theta && edge->a->theta < node->theta) ||
            (edge->a->theta == node->theta || edge->b->theta == node->theta)) {
            edges.erase(edges.begin() + i);
            return edge;
        }
    }
    // The node is within the Hull
    return nullptr;
}

// To increase runtime speed, we do not check for integrity dynamically when adding edges
bool Hull::checkIntegrity() {
    Edge* e = edges[0];
    Node* start_n = e->a;
    Node* current_n = e->b;
    Edge* current_e = e;
    std::unordered_set<Node*> visited = {e->a, e->b};
    bool found;
    while (current_n != start_n) {
        found = false; for (auto edge : edges) {
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

/* class NavMesh */

/* A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
void NavMesh::triangulate(std::vector<Coord*> coords) {
    if (coords.size() < 3) return;

    // INITIALIZATION
    m_mesh = TriangleMesh();
    Coord o = avgCoord(coords);
    std::vector<Node*> nodes;
    for (auto coord : coords) {
        nodes.emplace_back(coord->x, coord->y, o);
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
    Hull* frontier = new Hull(o, t->edges);
    i += 3;

    // TRIANGULATION
    while(i < nodes.size()) {
        // Create new triangle
        Edge* intersector = frontier->popIntersectingEdge(nodes[i]);
        Triangle* candidate = new Triangle(intersector, nodes[i]);
        Triangle* neighbour = intersector->triangle_ptrs[0];

        // Flip edges if the triangle does not meet the Delaunay condition
        Node* candidate_a = candidate->nodeOpositeToEdge(intersector);
        Node* neighbour_a = neighbour->nodeOpositeToEdge(intersector);
        std::vector<Edge*> candidate_edges = candidate->adjacentEdges(intersector);
        std::vector<Edge*> neighbour_edges = neighbour->adjacentEdges(intersector);
        float candidate_alpha = std::acos((std::pow(candidate_edges[0]->length, 2) + std::pow(candidate_edges[1]->length, 2) - std::pow(intersector->length, 2)) / (2 * candidate_edges[0]->length * candidate_edges[1]->length));
        float neighbour_alpha = std::acos((std::pow(candidate_edges[0]->length, 2) + std::pow(candidate_edges[1]->length, 2) - std::pow(intersector->length, 2)) / (2 * candidate_edges[0]->length * candidate_edges[1]->length));
        if (candidate_alpha + neighbour_alpha > M_PI) {
            // Flip edges
            delete intersector;
            auto pos = std::find(m_mesh.begin(), m_mesh.end(), neighbour);
            if (pos != m_mesh.end()) m_mesh.erase(m_mesh.begin() + std::distance(m_mesh.begin(), pos));
            neighbour_edges[0]->removeTrianglePtr(neighbour);
            neighbour_edges[1]->removeTrianglePtr(neighbour);
            // std::vector<Triangle*> neighbour_neighbours; 
            // for (auto t : neighbour_neighbours) {
            //     t.removeNeighbour(neighbour);
            // }
            neighbour = new Triangle(neighbour_edges[0], neighbour_a);
            candidate = new Triangle(neighbour_edges[1], neighbour_a);
            // for (auto t : neighbour_neighbours) {
            //     t.addNeighbour(neighbour);
            // }
            m_mesh.push_back(neighbour);
        }

        // Update mesh and hull
        // candidate->addNeighbour(neighbour);
        // neighbour->addNeighbour(candidate);
        m_mesh.push_back(candidate);
        frontier->edges.push_back(candidate_edges[0]);
        frontier->edges.push_back(candidate_edges[1]);
        
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

Coord NavMesh::avgCoord(std::vector<Coord*> coords) {
    Coord max = {0, 0};  // Assuming no negative values, as coordinates are always positive
    Coord min = {m_map_size.x, m_map_size.y};
    for (auto coord : coords) {
        if (coord->x > max.x) max.x = coord->x;
        if (coord->x < min.x) min.x = coord->x;
        if (coord->y > max.y) max.y = coord->y;
        if (coord->y < min.y) min.y = coord->y;
    }
    return {(max.x + min.x) / 2, (max.y + min.y) / 2};
}
