/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "nav_mesh.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_set>

using namespace adamant::logic::elements;
using namespace adamant::graphics;
using namespace adamant::graphics::elements;

// May throw InsufficientNodesException or FailedTriangulationException
NavMesh::NavMesh(std::vector<Terrain*> terrains, MapSize map_size): m_map_size{map_size} {
    triangulate(terrains);
    removeTrianglesWithin(terrains);
    populateNodes();
}

const char* NavMesh::InsufficientNodesException::what() const throw() {
    return "Less than 3 nodes were given.";
}

const char* NavMesh::FailedTriangulationException::what() const throw() {
    return "No triangles could be created.";
}

void NavMesh::drawFirstTriangle(std::vector<Node*>& nodes, Hull& frontier) {
    int i = 0;
    bool done = false;
    Triangle* t;
    while(!done && i < nodes.size()) {
        for (auto j=i+1; j<nodes.size(); ++j) {
            for (auto k=j+1; k<nodes.size(); ++k) {
                if (j <= k) {
                    try {
                        t = new Triangle(nodes[i], nodes[j], nodes[k]);
                    } catch (const Triangle::IllegalTriangleException &e) {
                        continue;
                    }
                    nodes.erase(nodes.begin() + k);
                    nodes.erase(nodes.begin() + j);
                    nodes.erase(nodes.begin() + i);
                    m_mesh.push_back(t);
                    done = true;
                    goto end;
                }    
            }
        }
        ++i;
        end:;
    }

    // Failed creating initial triangle
    if (m_mesh.empty()) throw FailedTriangulationException();

    // Order the edges; left and right are considered by looking from the origin of the hull
    t->defineNeighboursFromCenter(m_origin);
    frontier.edges.push_back(t->edges[0]);
    frontier.edges.push_back(t->edges[1]);
    frontier.edges.push_back(t->edges[2]);
}

Triangle* NavMesh::legalize(Triangle* candidate) {
    int i = 0;
    // TODO: Does not consider legalizations after legalizing once
    while (i < 3) {
        Edge* e = candidate->edges[i++];
        Triangle* neighbour = nullptr;
        for (Polygon* s : e->shape_ptrs) {
            // TODO: What if the original terrain was a triangle?
            if (s->type == Shape::triangle && s != candidate) neighbour = (Triangle*) s;
        }
        if (neighbour == nullptr) continue;
        // Legalize the triangle if it infringes the Delaunay condition
        float candidate_alpha = candidate->angleOppositeToEdge(e);
        float neighbour_alpha = neighbour->angleOppositeToEdge(e);
        if (candidate_alpha + neighbour_alpha > M_PI) {
            // Flip edges
            Node* candidate_n = candidate->nodeOppositeToEdge(e);
            std::vector<Edge*> neighbour_edges = neighbour->adjacentEdges(e);
            Triangle* new_neighbour;
            Triangle* new_candidate;
            try {
                new_neighbour = new Triangle(neighbour_edges[0], candidate_n);
            } catch (Triangle::IllegalTriangleException e) {
                continue;
            }
            try {
                new_candidate = new Triangle(neighbour_edges[1], candidate_n);
            } catch (Triangle::IllegalTriangleException e) {
                delete new_neighbour;
                continue;
            }
            // Remove neighbour from the mesh 
            auto pos_m = std::find(m_mesh.begin(), m_mesh.end(), neighbour);
            if (pos_m != m_mesh.end()) m_mesh.erase(pos_m);
            delete e;
            delete neighbour;
            delete candidate;
            candidate = new_candidate;
            // Add the new neighbour to the mesh
            m_mesh.push_back(new_neighbour);
            break;
        }
    }
    return candidate;
}

void NavMesh::sideWalk(Edge* edge, Edge* neighbour, Hull& frontier) {
    while(edge->angleWith(neighbour) < M_PI / 2) {
        Triangle* t;
        try { 
            t = new Triangle(edge, neighbour);
        } catch (const Triangle::IllegalTriangleException &e) {
            break;
        }

        Edge* new_e;
        if (t->edges[0] != edge && t->edges[0] != neighbour) new_e = t->edges[0];
        else if (t->edges[1] != edge && t->edges[1] != neighbour) 
            new_e = t->edges[1];
        else new_e = t->edges[2];
        frontier.edges.push_back(new_e);

        // Remove prev. edges from frontier
        auto pos = std::find(frontier.edges.begin(), frontier.edges.end(), edge);
        if (pos != frontier.edges.end()) frontier.edges.erase(pos);
        pos = std::find(frontier.edges.begin(), frontier.edges.end(), neighbour);
        if (pos != frontier.edges.end()) frontier.edges.erase(pos);

        // Update left & right
        if (neighbour == edge->left) {
            new_e->left = edge->left->left;
            new_e->right = edge->right;
            edge->left->left->right = new_e;
            edge->right->left = new_e;
            edge->left->left = nullptr;
            edge->left->right = nullptr;
            edge->left = nullptr;
            edge->right = nullptr;
        } else {
            new_e->left = edge->left;
            new_e->right = edge->right->right;
            edge->left->right = new_e;
            edge->right->right->left = new_e;
            edge->right->left = nullptr;
            edge->right->right = nullptr;
            edge->left = nullptr;
            edge->right = nullptr;
        }

        t = legalize(t);
        m_mesh.push_back(t);
        if (neighbour == edge->left) neighbour = new_e->left;
        else neighbour = new_e->right;
        edge = new_e;
    } 
}

void NavMesh::finalWalk(Edge* initial_edge, Hull& frontier) {
    Edge* edge = initial_edge;
    do {
        if (edge->angleWith(edge->left) < M_PI - std::numeric_limits<double>::epsilon()) {
            Triangle* t;
            try { 
                t = new Triangle(edge, edge->left);
            } catch (const Triangle::IllegalTriangleException &e) {
                edge = edge->left;
                continue;
            }

            Edge* new_e;
            if (t->edges[0] != edge && t->edges[0] != edge->left) new_e = t->edges[0];
            else if (t->edges[1] != edge && t->edges[1] != edge->left) new_e = t->edges[1];
            else new_e = t->edges[2];
            frontier.edges.push_back(new_e);

            // Remove prev. edges from frontier
            auto pos = std::find(frontier.edges.begin(), frontier.edges.end(), edge);
            if (pos != frontier.edges.end()) frontier.edges.erase(pos);
            pos = std::find(frontier.edges.begin(), frontier.edges.end(), edge->left);
            if (pos != frontier.edges.end()) frontier.edges.erase(pos);

            // Update left & right
            new_e->left = edge->left->left;
            new_e->right = edge->right;
            edge->left->left->right = new_e;
            edge->right->left = new_e;
            edge->left->left = nullptr;
            edge->left->right = nullptr;

            // Update initial edge so we do not loop forever
            if (initial_edge == edge || initial_edge == edge->left) {
                initial_edge = new_e->right;
            }

            // Finish updating left & right
            edge->left = nullptr;
            edge->right = nullptr;

            t = legalize(t);
            m_mesh.push_back(t);
            edge = new_e;
        } else {
            edge = edge->left;
        }
    } while (edge != initial_edge);
}

/* This is a custom implementation of:
 * A faster circle-sweep Delaunay triangulation algorithm
 * Ahmad Biniaz and Gholamhossein Dastghaibyfard
 * http://cglab.ca/~biniaz/papers/Sweep%20Circle.pdf */
/* This implementation features restricted areas, corresponding to terrain
 * and other elements of the game */
void NavMesh::triangulate(std::vector<Terrain*>& terrains) {

    // Get nodes
    for (Terrain* t : terrains) {
        for (Node* n : t->getShape()->nodes) {
            m_nodes.push_back(n);
        }       
    }
    if (m_nodes.size() < 3) throw InsufficientNodesException();

    m_mesh = TriangleMesh();
    m_origin = avgCoord(m_nodes);
    Hull frontier = Hull(m_origin);

    // Add corner nodes
    Node* n = new Node({0, 0}, m_origin);
    m_nodes.push_back(n);
    n = new Node({0, m_map_size.y}, m_origin);
    m_nodes.push_back(n);
    n = new Node({m_map_size.x, m_map_size.y}, m_origin);
    m_nodes.push_back(n);
    n = new Node({m_map_size.x, 0}, m_origin);
    m_nodes.push_back(n);
    std::vector<Node*> disconnected = m_nodes;
    for (Node* n : disconnected) {
        n->setOrigin(m_origin);
    }
    // TODO: Either just use nodes, or make disconnected a queue
    std::sort(disconnected.begin(), disconnected.end(), Node::RComparator());

    // Try all possible initial triangles in order of distance to the origin
    try {
        drawFirstTriangle(disconnected, frontier);
    } catch (const FailedTriangulationException &e) {
        throw e;
    }

    // Triangulate all nodes until none are disconnected
    while(!disconnected.empty()) {
        n = disconnected[0];
        // Create new triangle
        Edge* intersector = frontier.getIntersectingEdge(n);
        if (intersector == nullptr) {
            disconnected.erase(disconnected.begin());
            continue;
        } 
        Triangle* candidate;
        try {
            candidate = new Triangle(intersector, n);
        } catch (const Triangle::IllegalTriangleException &e) {
            /* Attempt to create triangles with the right and left edge of intersector
             * One of them is guaranteed to not be collinear with n, because
             * we return the closest intersecting edge to n. Both of them are also
             * guaranteed to form a triangle with n which does not intersect with
             * other triangles, since n and the intersector are collinear */
            try {
                candidate = new Triangle(intersector->left, n);
                intersector = intersector->left;
            } catch (const Triangle::IllegalTriangleException &e) {
                try {
                    candidate = new Triangle(intersector->right, n);
                    intersector = intersector->right;
                } catch (const Triangle::IllegalTriangleException &e) {
                    // Should not happen
                    disconnected.erase(disconnected.begin());
                    continue;
                }
            }
        }
        
        auto pos = std::find(frontier.edges.begin(), frontier.edges.end(), intersector);
        frontier.edges.erase(pos);
        disconnected.erase(disconnected.begin());
        
        /* Update frontier. We need the intersector as reference, but it may be deleted while
         * legalizing, so we do this before legalizing */
        std::vector<Edge*> new_frontier = candidate->adjacentEdges(intersector);
        Node* left_n = intersector->commonNodeWith(intersector->left);
        Edge* left_edge = new_frontier[0]->a == left_n || new_frontier[0]->b == left_n ?
                          new_frontier[0] : new_frontier[1];
        Edge* right_edge = new_frontier[0] == left_edge ? new_frontier[1] : new_frontier[0];
        left_edge->left = intersector->left;
        left_edge->right = right_edge;
        right_edge->left = left_edge;
        right_edge->right = intersector->right;
        intersector->left->right = left_edge;
        intersector->right->left = right_edge;
        intersector->left = nullptr;
        intersector->right = nullptr;
        frontier.edges.push_back(left_edge);
        frontier.edges.push_back(right_edge);
        
        // Update mesh
        candidate = legalize(candidate);
        m_mesh.push_back(candidate);

        sideWalk(left_edge, left_edge->left, frontier);
        sideWalk(right_edge, right_edge->right, frontier);

        // TODO: Remove basins
    }

    Edge* initial_edge = frontier.edges[0];
    finalWalk(initial_edge, frontier);
}

void NavMesh::removeTrianglesWithin(std::vector<Terrain*>& terrains) {
    for (auto i=0; i<m_mesh.size(); ++i) {
        for (Edge* e : m_mesh[i]->edges) {
            if (e->a->isRestrictedWith(e->b)) {
                m_mesh.erase(m_mesh.begin() + i--);
                break;
            }   
        }
    }
}

void NavMesh::populateNodes() {
    std::unordered_set<Edge*> visited;
    for (Triangle* t : m_mesh) {
        // Add intermediate nodes at each edge
        for (Edge* e : t->edges) {
            if (visited.find(e) == visited.end()) {
                visited.insert(e);
                std::vector<int_fast16_t> xs = {e->a->coord.x, e->b->coord.x};
                std::vector<int_fast16_t> ys = {e->a->coord.y, e->b->coord.y};
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
        // Add middle node
        Coord center = t->getCenter();
        Node* n = new Node(center, m_origin);
        m_nodes.push_back(n);
    }
}

Coord NavMesh::avgCoord(std::vector<Node*>& nodes) const {
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

MapSize NavMesh::getMapSize() const {
    return m_map_size;
}

TriangleMesh NavMesh::getMesh() const {
    return m_mesh;
}

std::vector<Node*> NavMesh::getNodes() const {
    return m_nodes;
}
