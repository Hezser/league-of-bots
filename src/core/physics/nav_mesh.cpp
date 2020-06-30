#include "nav_mesh.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_set>

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

bool Node::ThetaComparator::operator() (const Node& lhs, const Node& rhs) {
    return lhs.theta >= rhs.theta;
}

/* class NodePriorityQueue */

/*
int NodePriorityQueue::getIndexOf(Node* node) {
    
}
*/

/* struct Triangle */

// TODO: At the creation of each triangle, check that the 3 points are not in line
Triangle::Triangle(Node* a, Node* b, Node* c) {
    nodes = {a, b, c};
    edges = {new Edge(a, b, this), new Edge(b, c, this), new Edge(c, a, this)};
}

/* struct Hull */

Hull::Hull(Coord o) {
    this->o = o;
}

Hull::Hull(Coord o, std::vector<Edge*> edges) {
    this->o = o;
    this->edges = edges;
}

Edge* Hull::intersectingEdge(Node* node) {
    for (auto edge : edges) {
        if ((edge->a->theta > node->theta && edge->b->theta < node->theta) || 
            (edge->b->theta > node->theta && edge->a->theta < node->theta) ||
            (edge->a->theta == node->theta || edge->b->theta == node->theta)) {
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
    Hull* frontier = new Hull(o, {t->edges[0], t->edges[1], t->edges[2]});
    i += 3;

    // TRIANGULATION
    while(i < nodes.size()) {
        // Create new triangle
        Edge* intersector = frontier->intersectingEdge(nodes[i]);
        Triangle* new_triangle = new Triangle(intersector->a, intersector->b, nodes[i]);
        Triangle* neighbour = intersector->triangle_ptr;
        new_triangle->addNeighbor(neighbour);
        m_mesh.push_back(new_triangle);

        // TODO: Flip edges if needed

        // TODO: Update hull
        
        // TODO: Remove basins

        i++;
    }

    // FINALIZATION
    // Deal with special initialization case
    if (o_node != nullptr) {
        m_mesh.erase(m_mesh.begin());
        // TODO: set up neighbors
        m_mesh.emplace_back(nodes[0], nodes[1], o_node);
        m_mesh.emplace_back(nodes[1], nodes[2], o_node);
        m_mesh.emplace_back(nodes[0], nodes[2], o_node);
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
