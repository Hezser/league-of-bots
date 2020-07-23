#include "nav_mesh.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>
#include <unordered_set>

/* class NavMesh */

// May throw InsufficientNodesException or FailedTriangulationException
NavMesh::NavMesh(std::vector<Terrain*> terrains, MapSize map_size): m_map_size{map_size} {
    triangulate(terrains);
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
        if (e->shape_ptrs.size() != 2) continue;
        std::cout << "Size of e->shape_ptrs = " << e->shape_ptrs.size() << std::endl;
        auto s = (e->shape_ptrs[0] == candidate ? e->shape_ptrs[1] : e->shape_ptrs[0]);
        if (s->type != Shape::triangle) continue;
        Triangle* neighbour = (Triangle*) s;
        // Legalize the triangle if it infringes the Delaunay condition
        float candidate_alpha = candidate->angleOppositeToEdge(e);
        float neighbour_alpha = neighbour->angleOppositeToEdge(e);
        if (candidate_alpha + neighbour_alpha > M_PI) {
            std::cout << "Flipping edges\n";
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
    std::cout << "\t87\n";
    return candidate;
}

/* This is a custom implementation of:
 * A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
/* This implementation features restricted areas, corresponding to terrain
 * and other elements of the game */
void NavMesh::triangulate(std::vector<Terrain*> terrains) {

    // INITIALIZATION
    
    // Get nodes
    for (Terrain* t : terrains) {
        for (Node* n : t->getShape()->nodes) {
            m_nodes.push_back(n);
        }       
    }
    
    m_mesh = TriangleMesh();
    m_origin = avgCoord(m_nodes);
    if (m_nodes.size() < 3) throw InsufficientNodesException();

    std::vector<Node*> disconnected = m_nodes;
    for (Node* n : disconnected) {
        n->setOrigin(m_origin);
    }
    std::sort(disconnected.begin(), disconnected.end(), Node::RComparator());

    Hull* frontier = new Hull(m_origin);
    for (Terrain* t : terrains)  {
        for (Edge* e : t->getShape()->edges) {
            frontier->edges.push_back(e);
        }
    }

    // Try all possible initial triangles in order of distance to the origin
    int i = 0;
    bool done = false;
    Triangle* t;
    while(!done && i < disconnected.size()) {
        for (auto j=i+1; j<disconnected.size(); j++) {
            for (auto k=j+1; k<disconnected.size(); k++) {
                if (j <= k) {
                    try {
                        // TODO FIX: WHAT IF THE CENTER IS WITHIN TERRAIN?
                        t = new Triangle(disconnected[i], disconnected[j], disconnected[k]);
                    } catch (const IllegalShapeException &e) {
                        std::cout << "Exception thrown when creating { (" << 
                                disconnected[i]->coord.x << "," << disconnected[i]->coord.y <<
                                "), (" << disconnected[j]->coord.x << "," << 
                                disconnected[j]->coord.y << "), (" << disconnected[k]->coord.x 
                                << "," << disconnected[k]->coord.y << ") }: " <<
                                e.what() << std::endl;
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
    if (m_mesh.empty()) throw FailedTriangulationException();

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
    frontier->edges.push_back(anchor_e);
    frontier->edges.push_back(left_e);
    frontier->edges.push_back(right_e);

    // TRIANGULATION
    
    Node* n;
    while(!disconnected.empty()) {
        std::cout << "1\n";
        n = disconnected[0];
        // Create new triangle
        /* std::cout << "Mesh has " << m_mesh.size() << std::endl; */
        /* std::cout << "Hull (edges) has " << frontier->edges.size() << std::endl; */
        /* std::cout << "Nodes has " << m_nodes.size() << std::endl; */
        /* std::cout << "Origin is (" << m_origin.x << ", " << m_origin.y << ")\n"; */
        Edge* intersector = frontier->popIntersectingEdge(n);
        std::cout << "2\n";
        if (intersector == nullptr) {
            disconnected.erase(disconnected.begin());
            std::cout << "Intersector is null\n";
            continue;
        } 
        Triangle* candidate;
        try {
            candidate = new Triangle(intersector, n);
            std::cout << "3\n";
        } catch (Edge::IllegalEdgeException) {
            disconnected.erase(disconnected.begin());
            std::cout << "Tried to violate a shape\n";
            continue;
        } catch (Triangle::IllegalTriangleException) {
            std::cout << "Collinear triangle\n";
            /* Attempt to create triangles with the right and left edge of intersector
             * One of them is guaranteed to not be collinear with n, because
             * we return the closest intersecting edge to n. Both of them are also
             * guaranteed to form a triangle with n which does not intersect with
             * other triangles, since n and the intersector are collinear */
            try {
                candidate = new Triangle(intersector->left, n);
                std::cout << "4\n";
            } catch (const IllegalShapeException &e) {
                std::cout << "Collinear triangle, trying somethig else...\n";
                try {
                    candidate = new Triangle(intersector->right, n);
                    std::cout << "5\n";
                } catch (const IllegalShapeException &e) {
                    // One of the right and left edges of the intersector has restricted nodes
                    std::cout << "Collinear triangle, eventually a shape was violated\n";
                    disconnected.erase(disconnected.begin());
                    continue;
                }
            }
        }
        std::cout << "6\n";

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

        std::cout << "7\n";
        // Left-side walk
        if (left_edge->left != nullptr) {
            while(left_edge->angleWith(left_edge->left) < M_PI / 2) {
                Node* left_n = left_edge->left->a == left_edge->a ||
                               left_edge->left->a == left_edge->b ?
                               left_edge->left->b : left_edge->left->a;
                Triangle* t;
                try { 
                    t = new Triangle(left_edge, left_edge->left, left_n);
                } catch (const IllegalShapeException &e) {
                    break;
                }

                Edge* new_e;
                if (t->edges[0] != left_edge && t->edges[0] != left_edge->left) new_e = t->edges[0];
                else if (t->edges[1] != left_edge && t->edges[1] != left_edge->left) 
                    new_e = t->edges[1];
                else new_e = t->edges[2];
                frontier->edges.push_back(new_e);
                auto pos = std::find(frontier->edges.begin(), frontier->edges.end(), left_edge);
                if (pos != frontier->edges.end()) {
                    frontier->edges.erase(frontier->edges.begin() +
                            std::distance(frontier->edges.begin(), pos));
                }
                pos = std::find(frontier->edges.begin(), frontier->edges.end(), left_edge->left);
                if (pos != frontier->edges.end()) {
                    frontier->edges.erase(frontier->edges.begin() +
                            std::distance(frontier->edges.begin(), pos));
                }

                t = legalize(t);
                m_mesh.push_back(t);
                left_edge = left_edge->left;
                if (left_edge->left == nullptr) break;
            } 
        }
        std::cout << "8\n";

        // Right-side walk
        if (right_edge->right != nullptr) {
            while(right_edge->angleWith(right_edge->right) < M_PI / 2) {
                Node* right_n = right_edge->right->a == right_edge->a ||
                               right_edge->right->a == right_edge->b ?
                               right_edge->right->b : right_edge->right->a;
                Triangle* t;
                try {
                    t = new Triangle(right_edge, right_edge->right, right_n);
                } catch (const IllegalShapeException &e) {
                    break;
                }

                Edge* new_e;
                if (t->edges[0] != right_edge && t->edges[0] != right_edge->right) 
                    new_e = t->edges[0];
                else if (t->edges[1] != right_edge && t->edges[1] != right_edge->right) 
                    new_e = t->edges[1];
                else new_e = t->edges[2];
                frontier->edges.push_back(new_e);
                auto pos = std::find(frontier->edges.begin(), frontier->edges.end(), right_edge);
                if (pos != frontier->edges.end()) {
                    frontier->edges.erase(frontier->edges.begin() +
                            std::distance(frontier->edges.begin(), pos));
                }
                pos = std::find(frontier->edges.begin(), frontier->edges.end(), right_edge->right);
                if (pos != frontier->edges.end()) {
                    frontier->edges.erase(frontier->edges.begin() +
                            std::distance(frontier->edges.begin(), pos));
                }

                t = legalize(t);
                m_mesh.push_back(t);
                right_edge = right_edge->right;
                if (right_edge->right == nullptr) break;
            }
        }
        std::cout << "9\n";

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
        } catch(const IllegalShapeException &e) {
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
                    Node* n = new Node({xs[0] + (diff_x / 2), ys[0] + (diff_y / 2)}, m_origin);
                    m_nodes.push_back(n);
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
