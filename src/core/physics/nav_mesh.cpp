#include "nav_mesh.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <queue>
#include <unordered_set>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

/* class NavMesh */

// May throw InsufficientNodesException or FailedTriangulationException
NavMesh::NavMesh(std::vector<Terrain*> terrains, MapSize map_size): m_map_size{map_size} {
    m_nodes = getTerrainNodes(terrains);
    triangulate();
    populateNodes();
}

const char* NavMesh::InsufficientNodesException::what() const throw() {
    return "Less than 3 nodes were given.";
}

const char* NavMesh::FailedTriangulationException::what() const throw() {
    return "No triangles could be created.";
}

Triangle* NavMesh::legalize(Triangle* candidate) {
    for (Edge* e : candidate->edges) {
        if (e->shape_ptrs.size() < 2) continue;
        Triangle* neighbour = (Triangle*) ((Triangle*) e->shape_ptrs[0] == candidate ?
                e->shape_ptrs[1] : e->shape_ptrs[0]);
        // Legalize the triangle if it infringes the Delaunay condition
        float candidate_alpha = candidate->angleOppositeToEdge(e);
        float neighbour_alpha = neighbour->angleOppositeToEdge(e);
        if (candidate_alpha + neighbour_alpha > M_PI) {
            // Flip edges
            Node* candidate_a = candidate->nodeOppositeToEdge(e);
            std::vector<Edge*> neighbour_edges = neighbour->adjacentEdges(e);
            Triangle* new_neighbour;
            Triangle* new_candidate;
            try {
                new_neighbour = new Triangle(neighbour_edges[0], candidate_a);
                new_candidate = new Triangle(neighbour_edges[1], candidate_a);
            } catch (Edge::IllegalEdgeException e) {
                continue;
            } catch (Triangle::IllegalTriangleException e) {
                continue;
            }
            // Remove neighbour and candidate
            auto pos = std::find(m_mesh.begin(), m_mesh.end(), neighbour);
            if (pos != m_mesh.end()) {
                m_mesh.erase(m_mesh.begin() + std::distance(m_mesh.begin(), pos));
            }
            delete e;
            delete neighbour;
            delete candidate;
            candidate = new_candidate;
            // Add the new neighbour to the mesh
            m_mesh.push_back(new_neighbour);
        }
    }
    return candidate;
}

/* This is a custom implementation of:
 * A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
/* This implementation features restricted areas, corresponding to terrain
 * and other elements of the game */
void NavMesh::triangulate() {

    // INITIALIZATION

    m_mesh = TriangleMesh();
    Coord origin = avgCoord(m_nodes);
    if (m_nodes.size() < 3) throw InsufficientNodesException();

    std::vector<Node*> disconnected = m_nodes;
    std::sort(disconnected.begin(), disconnected.end(), Node::RComparator());

    // Try all possible initial triangles in order of distance to the origin
    int i = 0;
    bool done = false;
    Triangle* t;
    while(!done && i < disconnected.size()) {
        for (auto j=i+1; j<disconnected.size(); j++) {
            for (auto k=j+1; k<disconnected.size(); k++) {
                if (j <= k) {
                    try {
                        t = new Triangle(disconnected[i], disconnected[j], disconnected[k]);
                    } catch (ShapeException) {
                        continue;
                    }
                    disconnected.erase(disconnected.begin() + k);
                    disconnected.erase(disconnected.begin() + j);
                    disconnected.erase(disconnected.begin() + i);
                    m_mesh.push_back(t);
                    done = true;
                    goto end;
                }    
            }
        }
        i++;
        end:;
    }

    // Failed creating initial triangle
    if (m_mesh.empty()) FailedTriangulationException();

    // Order the edges; left and right are considered by looking from the origin of the hull
    Edge* anchor_e = t->edges[0];
    Edge* left_e = anchor_e->hasAtLeft(t->edges[1]) ? t->edges[1] : t->edges[2];
    Edge* right_e = t->edges[1] == left_e ? t->edges[2] : t->edges[1];
    anchor_e->left = left_e;
    anchor_e->right = right_e;
    left_e->left = right_e;
    left_e->right = anchor_e;
    right_e->left = anchor_e;
    right_e->right = left_e;
    Hull* frontier = new Hull(origin, {anchor_e, left_e, right_e});

    // TRIANGULATION
    
    Node* n;
    while(!disconnected.empty()) {
        n = disconnected[0];
        // Create new triangle
        Edge* intersector = frontier->popIntersectingEdge(n);
        Triangle* candidate;
        try {
            candidate = new Triangle(intersector, n);
        } catch (Edge::IllegalEdgeException) {
            disconnected.erase(disconnected.begin());
            continue;
        } catch (Triangle::IllegalTriangleException) {
            /* Attempt to create triangles with the right and left edge of intersector
             * One of them is guaranteed to not be collinear with n, because
             * we return the closest intersecting edge to n. Both of them are also
             * guaranteed to form a triangle with n which does not intersect with
             * other triangles, since n and the intersector are collinear */
            try {
                candidate = new Triangle(intersector->left, n);
            } catch (ShapeException) {
                try {
                    candidate = new Triangle(intersector->right, n);
                } catch (ShapeException) {
                    // One of the right and left edges of the intersector has restricted nodes
                    disconnected.erase(disconnected.begin());
                    continue;
                }
            }
        }

        disconnected.erase(disconnected.begin());
        
        // Update mesh and frontier
        candidate = legalize(candidate);
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
            Triangle* t;
            try {
                t = new Triangle(left_edge, left_edge->left, left_n);
            } catch (ShapeException) {
                left_edge = left_edge->left;
                continue;
            }
            t = legalize(t);
            m_mesh.push_back(t);
            left_edge = left_edge->left;
        }

        // Right-side walk
        while(right_edge->angleWith(right_edge->right) < M_PI / 2) {
            Node* right_n = right_edge->right->a == right_edge->a ||
                           right_edge->right->a == right_edge->b ?
                           right_edge->right->b : right_edge->right->a;
            Triangle* t;
            try {
                t = new Triangle(right_edge, right_edge->right, right_n);
            } catch (ShapeException) {
                right_edge = right_edge->right;
                continue;
            }
            t = legalize(t);
            m_mesh.push_back(t);
            right_edge= right_edge->right;
        }

        // TODO: Remove basins
    }

    // FINALIZATION 
        
    // Check for acute angles in the frontier (make a concave hull convex) 
    for (Edge* e : frontier->edges) {
        Node* left_n = e->left->a == e->a ||
                       e->left->a == e->b ?
                       e->left->b : e->left->a;
        Triangle* t;
        try {
            t = new Triangle(e, e->left, left_n);
        } catch(ShapeException) {
            continue;
        }
        t = legalize(t);
        m_mesh.push_back(t);
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
                    m_nodes.emplace_back(xs[0] + (diff_x / 2), ys[0] + (diff_y / 2), m_hull->origin);
                }
            }
        }
    }
}

Coord NavMesh::avgCoord(std::vector<Node*> nodes) {
    Coord max = {0, 0};  // Assuming no negative values, as coordinates are always positive
    Coord min = {m_map_size.x, m_map_size.y};
    for (auto n : nodes) {
        if (n->coord.x > max.x) max.x = n->coord.x;
        if (n->coord.x < min.x) min.x = n->coord.x;
        if (n->coord.y > max.y) max.y = n->coord.y;
        if (n->coord.y < min.y) min.y = n->coord.y;
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
