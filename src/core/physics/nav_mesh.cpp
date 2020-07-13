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

NavMesh::NavMesh(std::vector<Terrain*> terrains, MapSize map_size): m_map_size{map_size} {
    triangulate(calculateCoords(terrains));
    populateNodes();
}

const char* NavMesh::InsufficientNodesException::what() const throw() {
    return "Less than 3 nodes were given.";
}

std::vector<Coord> NavMesh::calculateCoords(std::vector<Terrain*> terrains) {
    // TODO: Implement  
}

/* /1* I developed this algorithm to deal with nodes that could not be initially triangulated */
/*  * due to restrictions on intra-terrain edge constructions *1/ */
/* void NavMesh::connectNode(Node* node) { */
/*     if (m_nodes.size() < 3) return; */
/*     // Project nodes to the center of the node to connect */
/*     std::vector<Node*> projections = m_nodes; */
/*     auto pos = std::find(projections.begin(), projections.end(), node); */
/*     if (pos != projections.end()) { */
/*         projections.erase(projections.begin() + std::distance(projections.begin(), pos)); */
/*     } */
/*     for (auto n : projections) { */
/*         n->setOrigin(node->coord); */
/*     } */

/*     // Order nodes: first by r, then by theta */
/*     std::sort(projections.begin(), projections.end(), Node::RComparator()); */

/*     // Create a barrier */
/*     Barrier* barrier = new Barrier(); */
/*     int i = 0; */
/*     while(!barrier->isClosed() && i < projections.size()) { */
/*         barrier.addNode(projections[i]); */
/*         i++; */
/*     } */
    
/*     // Create triangles with the barrier */
/*     std::priority_queue<Node*> connections = barrier.getNodes(); */
/*     Node* first = barrier.top(); */
/*     while(connections.size() > 0 && first != nullptr) { */
/*         // Check (last, first) on the last iteration */
/*         if (connections.size() == 1 && connections.top() != first) { */ 
/*             connections.push(first); */
/*             first = nullptr; */
/*         } */
/*         Node* n = connections.top(); */
/*         connections.pop(); */
/*         Node* m = connections.top(); */
/*         try { */
/*             Triangle* t = nullptr; */
/*             Edge* e = n->getEdgeWith(m); */
/*             if (e == nullptr) { */
/*                 t = new Triangle(node, n, m); */
/*             } else { */
/*                 t = new Triangle(e, node); */
/*             } */
/*             legalize(t); */
/*             m_mesh.push_back(t); */
/*         } catch (Edge::IllegalEdgeException e) { */
/*             // m cannot be further used */
/*             if (node->restricted.find(m) != node->restricted.end()) connections.pop(); */
/*             // Try again with {n, m+1} */
/*             if (node->restricted.find(n) == node->restricted.end()) connections.push(n); */
/*         } catch (Triangle::IllegalTriangleException e) { */
/*             // Continue */
/*         } */
/*     } */

/*     // Restore origin of nodes */
/*     for (auto n : m_nodes) { */
/*         n->setOrigin(m_hull->origin); */
/*     } */
/* } */

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
    // TODO: Throw some exception
    if (coords.size() < 3) return;

    // INITIALIZATION
    m_mesh = TriangleMesh();
    Coord origin = avgCoord(coords);
    
    for (auto coord : coords) {
        m_nodes.emplace_back(coord.x, coord.y, origin);
    }

    std::sort(m_nodes.begin(), m_nodes.end(), Node::RComparator());
    int i = 0;

    // Special case where a node is at the origin (so that node[0]->r == 0)
    Node* origin_n = nullptr;
    if (m_nodes[0]->coord.x == origin.x && m_nodes[0]->coord.y == origin.y) {
        origin_n = m_nodes[0];
        m_nodes.erase(m_nodes.begin());
    }
    
    // Create first triangle
    bool done = false;
    int j = 0;
    Triangle* t;
    while(!done && j < m_nodes.size()) {
        try {
            t = new Triangle(m_nodes[j], m_nodes[(j+1) % m_nodes.size()], 
                             m_nodes[(j+2) % m_nodes.size()]);
        } catch (ShapeException e) {
            try {
                t = new Triangle(m_nodes[j], m_nodes[(j+1) % m_nodes.size()], 
                                 m_nodes[(j+3) % m_nodes.size()]);
            } catch (ShapeException e) {
                try {
                    t = new Triangle(m_nodes[j], m_nodes[(j+3) % m_nodes.size()], 
                                     m_nodes[(j+2) % m_nodes.size()]);
                } catch (ShapeException e) {
                    j++;
                    continue;
                }
            }
        }
        done = true;
        m_mesh.push_back(t);
    }

    // Failed creating initial triangle
    // TODO: Throw some exception
    if (m_mesh.empty()) return;

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
    Hull* frontier = new Hull(origin, {anchor_e, left_e, right_e});

    i += 3;

    // TRIANGULATION
    while(i < m_nodes.size()) {
        // Create new triangle
        Edge* intersector = frontier->popIntersectingEdge(m_nodes[i]);
        Triangle* candidate;
        try {
            candidate = new Triangle(intersector, m_nodes[i]);
        } catch (Edge::IllegalEdgeException) {
            i++;
            continue;
        } catch (Triangle::IllegalTriangleException) {
            /* Attempt to create triangles with the right and left edge of intersector
             * One of them is guaranteed to not be collinear with m_nodes[i], because
             * we return the closest intersecting edge to m_nodes[i]. Both of them are also
             * guaranteed to form a triangle with m_nodes[i] which does not intersect with
             * other triangles, since m_nodes[i] and the intersector are collinear */
            try {
                candidate = new Triangle(intersector->left, m_nodes[i]);
            } catch (ShapeException) {
                try {
                    candidate = new Triangle(intersector->right, m_nodes[i]);
                } catch (ShapeException) {
                    // One of the right and left edges of the intersector has restricted nodes
                    i++;
                    continue;
                }
            }
        }
        legalize(candidate);
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
            legalize(t);
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
            legalize(t);
            m_mesh.push_back(t);
            right_edge= right_edge->right;
        }

        // TODO: Remove basins

        i++;
    }

    // FINALIZATION 
        
    // Deal with special initialization case
    if (origin_n != nullptr) {
        Triangle* t = m_mesh[0];
        m_mesh.erase(m_mesh.begin());
        m_mesh.emplace_back(t->edges[0], origin_n);
        m_mesh.emplace_back(t->edges[1], origin_n);
        m_mesh.emplace_back(t->edges[2], origin_n);
    }

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
        legalize(t);
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
