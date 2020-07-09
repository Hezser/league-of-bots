#include "nav_mesh.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_set>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* TODO: A hull may be more efficient to implement with nodes,
 * not edges (easier to find left/right nodes) */
/* struct Hull */

Hull::Hull(Coord o) {
    this->o = o;
}

Hull::Hull(Coord o, std::vector<Edge*> edges) {
    this->o = o;
    this->edges = edges;
}

/* If there are >3 edges being collinear and a node intersects with them, the node needs to
 * get the closest edge to it (furtherst away from origin), which guarantees that the node can
 * make a non-collinear triangle with the left or right edge of the intersector */
Edge* Hull::popIntersectingEdge(Node* node) {
    int j = -1;
    for (int i=0; i<edges.size(); i++) {
        Edge* edge = edges[i];
        if ((edge->a->theta > node->theta && edge->b->theta < node->theta) || 
            (edge->b->theta > node->theta && edge->a->theta < node->theta) ||
            (edge->a->theta == node->theta)) {
            if (j == -1 || edge->avgR() > edges[j]->avgR()) j = i;
        }
    }

    // The node is within the Hull
    if (j == -1) return nullptr;

    Edge* intersector = edges[j];
    edges.erase(edges.begin() + j);
    return intersector;
}

// TODO: Can be improved now that we have left/right pointers
// To increase runtime speed, we do not check for integrity dynamically when adding edges
bool Hull::checkIntegrity() {
    Edge* e = edges[0];
    Node* start_n = e->a;
    Node* current_n = e->b;
    Edge* current_e = e;
    std::unordered_set<Node*> visited = {e->a, e->b};
    bool found;
    while (current_n != start_n) {
        found = false; 
        for (auto container : edges) {
            Edge* edge = container;
            if (edge == current_e) continue;
            if (edge->a == current_n) {
                if (visited.find(edge->b) != visited.end()) return false;  // Inner cycles
                visited.insert(edge->b);
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

NavMesh::NavMesh(std::vector<Terrain*> terrains, MapSize map_size): m_map_size{map_size} {
    triangulate(calculateCoords(terrains));
    populateNodes();
}

const char* NavMesh::InsufficientNodesException::what() const throw() {
    return "Less than 3 nodes were given.";
}

std::vector<Coord> NavMesh::calculateCoords(std::vector<Terrain*> terrains) {
       
}

void NavMesh::legalize(Triangle* t) {
    for (Edge* e : t->edges) {
        if (e->shape_ptrs.size() < 2) continue;
        Triangle* neighbour = (Triangle*) ((Triangle*) e->shape_ptrs[0] == t ?
                e->shape_ptrs[1] : e->shape_ptrs[0]);
        // Legalize the triangle if it infringes the Delaunay condition
        float candidate_alpha = t->angleOppositeToEdge(e);
        float neighbour_alpha = neighbour->angleOppositeToEdge(e);
        if (candidate_alpha + neighbour_alpha > M_PI) {
            // Flip edges
            Node* candidate_a = t->nodeOppositeToEdge(e);
            std::vector<Edge*> neighbour_edges = neighbour->adjacentEdges(e);
            Triangle* new_neighbour;
            try {
                new_neighbour = new Triangle(neighbour_edges[0], candidate_a);
                t = new Triangle(neighbour_edges[1], candidate_a);
            } catch (Edge::IllegalEdgeException e) {
                continue;
            } catch (Triangle::IllegalTriangleException e) {
                continue;
            }
            neighbour_edges[0]->removeShapePtr(neighbour);
            neighbour_edges[1]->removeShapePtr(neighbour);
            auto pos = std::find(m_mesh.begin(), m_mesh.end(), neighbour);
            if (pos != m_mesh.end()) {
                m_mesh.erase(m_mesh.begin() + std::distance(m_mesh.begin(), pos));
            }
            m_mesh.push_back(new_neighbour);
            delete e;
        }
    }   
}

/* This is a custom implementation of:
 * A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
void NavMesh::triangulate(std::vector<Coord> coords) {
    // TODO: This should be somewhere else
    if (coords.size() < 3) return;

    // INITIALIZATION
    // TODO: Include std::vector<Node*> restricted_nodes for each node
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
    if (nodes[0]->coord.x == o.x && nodes[0]->coord.y == o.y) {  // Since node[0]->r is a float, let's not deal with accuracy
        o_node = nodes[0];
        nodes.erase(nodes.begin());
    }
    
    // Create first triangle
    bool done = false;
    int j = i;
    Triangle* t;
    while(!done && j < nodes.size()) {
        try {
            t = new Triangle(nodes[j], nodes[(j+1) % nodes.size()], 
                             nodes[(j+2) % nodes.size()]);
        } catch (ShapeException e) {
            try {
                t = new Triangle(nodes[j], nodes[(j+1) % nodes.size()], 
                                 nodes[(j+3) % nodes.size()]);
            } catch (ShapeException e) {
                try {
                    t = new Triangle(nodes[j], nodes[(j+3) % nodes.size()], 
                                     nodes[(j+2) % nodes.size()]);
                } catch (ShapeException e) {
                    try {
                        t = new Triangle(nodes[(j+3) % nodes.size()], 
                                nodes[(j+1) % nodes.size()], nodes[(j+2) % nodes.size()]);
                    } catch (ShapeException e) {
                        j++;
                        continue;
                    }
                }
            }
        }
        done = true;
        m_mesh.push_back(t);
    }

    // Order the edges; left and right are considered looking from the origin of the hull
    Edge* anchor_e = t->edges[0];
    Edge* left_e = anchor_e->hasAtLeft(t->edges[1]) ? t->edges[1] : t->edges[2];
    Edge* right_e = t->edges[1] == left_e ? t->edges[2] : t->edges[1];
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
        Edge* intersector = frontier->popIntersectingEdge(nodes[i]);
        // TODO: Catch exceptions
        Triangle* candidate = new Triangle(intersector, nodes[i]);
        legalize(candidate);

        // Update mesh and hull
        m_mesh.push_back(candidate);
        std::vector<Edge*> new_frontier = candidate->adjacentEdges(intersector);
        Edge* left_edge = new_frontier[0]->hasAtLeft(new_frontier[1]) ? new_frontier[1] : new_frontier[0];
        Edge* right_edge = new_frontier[0] == left_edge ? new_frontier[1] : new_frontier[0];
        left_edge->left = intersector->left;
        left_edge->right = right_edge;
        right_edge->left = left_edge;
        right_edge->right = intersector->right;
        frontier->edges.push_back(left_edge);
        frontier->edges.push_back(right_edge);

        // Left-side walk
        while(left_edge->angleWith(left_edge->left) < M_PI / 2) {
            Node* left_n = left_edge->left->a == left_edge->a ||
                           left_edge->left->a == left_edge->b ?
                           left_edge->left->b : left_edge->left->a;
            Triangle* t = new Triangle(left_edge, left_edge->left, left_n);
            legalize(t);
            m_mesh.push_back(t);
            left_edge = left_edge->left;
        }

        // Right-side walk
        while(right_edge->angleWith(right_edge->right) < M_PI / 2) {
            Node* right_n = right_edge->right->a == right_edge->a ||
                           right_edge->right->a == right_edge->b ?
                           right_edge->right->b : right_edge->right->a;
            Triangle* t = new Triangle(right_edge, right_edge->right, right_n);
            legalize(t);
            m_mesh.push_back(t);
            right_edge= right_edge->right;
        }

        // TODO: Remove basins, implement when Hull is refactored to be based on nodes, not edges

        i++;
    }

    // FINALIZATION 
    // Check for acute angles in the frontier (make a concave hull convex) 
    // Necessary after we do left/right-side walking at each iteration?
    for (Edge* e : frontier->edges) {
        Node* left_n = e->left->a == e->a ||
                       e->left->a == e->b ?
                       e->left->b : e->left->a;
        Triangle* t = new Triangle(e, e->left, left_n);
        legalize(t);
        m_mesh.push_back(t);
    }
        
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
    // Add intermediate nodes at each edge
    std::unordered_set<Edge*> visited;
    for (Triangle* t : m_mesh) {
        for (Edge* e : t->edges) {
            if (visited.find(e) == visited.end()) {
                visited.insert(e);
                std::vector<int> xs = {e->a->coord.x, e->b->coord.x};
                std::vector<int> ys = {e->a->coord.y, e->b->coord.y};
                std::sort(xs.begin(), xs.end());
                std::sort(ys.begin(), ys.end());
                // Do not create intermediate nodes for small edges
                int diff_x = xs[1] - xs[0];
                int diff_y = ys[1] - ys[0];
                if (diff_x >= 10 && diff_y >= 10) {
                    m_nodes.emplace_back(xs[0] + (diff_x / 2), ys[0] + (diff_y / 2), m_hull->o);
                }
            }
        }
    }
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
