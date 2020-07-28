#include "nav_mesh.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <vector>
#include <queue>

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

Triangle* NavMesh::legalize(Triangle* candidate, Hull* frontier) {
    int i = 0;
    // TODO: Does not consider legalizations after legalizing once
    while (i < 3) {
        std::cout << "Legalizing!\n";
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
    if (m_nodes.size() < 3) throw InsufficientNodesException();

    m_mesh = TriangleMesh();
    m_origin = avgCoord(m_nodes);
    Hull* frontier = new Hull(m_origin);

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
    std::sort(disconnected.begin(), disconnected.end(), Node::RComparator());

    std::cout << "\nDisconnected (" << disconnected.size() << ")= [";
    for (auto i=0; i<disconnected.size(); i++) {
        std::cout << disconnected[i] << ", ";
    }
    std::cout << "]\n\n";

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
                    } catch (const Triangle::IllegalTriangleException &e) {
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
    t->defineNeighboursFromCenter(m_origin);
    frontier->edges.push_back(t->edges[0]);
    frontier->edges.push_back(t->edges[1]);
    frontier->edges.push_back(t->edges[2]);

    std::cout << "AFTER CREATING FIRST TRIANGLE ( " << frontier->edges.size() << " )------\n";
    for (int i=0; i<frontier->edges.size(); i++) {
        std::cout << "Edge " << i << " = " << frontier->edges[i] << "\n";
        std::cout << "\tA = " << frontier->edges[i]->a << "\n";
        std::cout << "\tB = " << frontier->edges[i]->b << "\n";
        std::cout << "\tLeft = " << frontier->edges[i]->left << "\n";
        std::cout << "\tRigh = " << frontier->edges[i]->right << "\n";
    }
    std::cout << "\n\n";
    
    // TRIANGULATION
    
    while(!disconnected.empty()) {
        std::cout << "\nDisconnected (" << disconnected.size() << ")= [";
        for (auto i=0; i<disconnected.size(); i++) {
            std::cout << disconnected[i] << ", ";
        }
        std::cout << "]\n\n";
        n = disconnected[0];
        std::cout << "\n\nNODE TO CONNECT = " << n << "\n";
        // Create new triangle
        Edge* intersector = frontier->getIntersectingEdge(n);
        if (intersector == nullptr) {
            std::cout << "**** Intersector is NULL!! ****\n";
            disconnected.erase(disconnected.begin());
            continue;
        } 
        std::cout << "Initial intersector = " << intersector << "\n";
        std::cout << "Initial intersector->left = " << intersector->left << "\n";
        std::cout << "Initial intersector->right = " << intersector->right << "\n";
        Triangle* candidate;
        try {
            candidate = new Triangle(intersector, n);
        } catch (Triangle::IllegalTriangleException) {
            /* Attempt to create triangles with the right and left edge of intersector
             * One of them is guaranteed to not be collinear with n, because
             * we return the closest intersecting edge to n. Both of them are also
             * guaranteed to form a triangle with n which does not intersect with
             * other triangles, since n and the intersector are collinear */
            try {
                std::cout << "**** TRYING INTERSECTOR'S LEFT!! ****\n";
                candidate = new Triangle(intersector->left, n);
                intersector = intersector->left;
            } catch (const Triangle::IllegalTriangleException &e) {
                try {
                    std::cout << "**** TRYING INTERSECTOR'S RIGHT!! ****\n";
                    candidate = new Triangle(intersector->right, n);
                    intersector = intersector->right;
                } catch (const Triangle::IllegalTriangleException &e) {
                    std::cout << "**** COULD NOT CREATE TRIANGLE!! ****\n";
                    // Should not happen
                    disconnected.erase(disconnected.begin());
                    continue;
                }
            }
        }
        
        auto pos = std::find(frontier->edges.begin(), frontier->edges.end(), intersector);
        frontier->edges.erase(pos);
        disconnected.erase(disconnected.begin());
        
        /* Update frontier. We need the intersector as reference, but it may be deleted while
         * legalizing, so we do this before legalizing */
        std::vector<Edge*> new_frontier = candidate->adjacentEdges(intersector);
        Node* left_n = intersector->commonNodeWith(intersector->left);
        Edge* left_edge = new_frontier[0]->a == left_n || new_frontier[0]->b == left_n ?
                          new_frontier[0] : new_frontier[1];
        Edge* right_edge = new_frontier[0] == left_edge ? new_frontier[1] : new_frontier[0];
        std::cout << "Intersector = " << intersector << "\n";
        std::cout << "\tA = " << intersector->a << "\n";
        std::cout << "\tB = " << intersector->b << "\n";
        std::cout << "\tLeft = " << intersector->left << "\n";
        std::cout << "\tRigh = " << intersector->right << "\n";
        std::cout << "Left = " << left_edge << "\n";
        std::cout << "\tA = " << left_edge->a << "\n";
        std::cout << "\tB = " << left_edge->b << "\n";
        std::cout << "\tLeft = " << left_edge->left << "\n";
        std::cout << "\tRigh = " << left_edge->right << "\n";
        std::cout << "Right = " << right_edge << "\n";
        std::cout << "\tA = " << right_edge->a << "\n";
        std::cout << "\tB = " << right_edge->b << "\n";
        std::cout << "\tLeft = " << right_edge->left << "\n";
        std::cout << "\tRigh = " << right_edge->right << "\n";
        left_edge->left = intersector->left;
        left_edge->right = right_edge;
        right_edge->left = left_edge;
        right_edge->right = intersector->right;
        intersector->left->right = left_edge;
        intersector->right->left = right_edge;
        intersector->left = nullptr;
        intersector->right = nullptr;
        frontier->edges.push_back(left_edge);
        frontier->edges.push_back(right_edge);
        
        // Update mesh
        candidate = legalize(candidate, frontier);
        m_mesh.push_back(candidate);
        std::cout << "AFTER UPDATING FRONTIER ( " << frontier->edges.size() << " )------\n";
        for (int i=0; i<frontier->edges.size(); i++) {
            std::cout << "Edge " << i << " = " << frontier->edges[i] << "\n";
            std::cout << "\tA = " << frontier->edges[i]->a << "\n";
            std::cout << "\tB = " << frontier->edges[i]->b << "\n";
            std::cout << "\tLeft = " << frontier->edges[i]->left << "\n";
            std::cout << "\tRigh = " << frontier->edges[i]->right << "\n";
        }
        std::cout << "\n\n";

        // Left-side walk
        while(left_edge->angleWith(left_edge->left) < M_PI / 2) {
            Triangle* t;
            try { 
                t = new Triangle(left_edge, left_edge->left);
            } catch (const Triangle::IllegalTriangleException &e) {
                break;
            }

            Edge* new_e;
            if (t->edges[0] != left_edge && t->edges[0] != left_edge->left) new_e = t->edges[0];
            else if (t->edges[1] != left_edge && t->edges[1] != left_edge->left) 
                new_e = t->edges[1];
            else new_e = t->edges[2];
            frontier->edges.push_back(new_e);

            std::cout << "left_edge = " << left_edge << "\n";
            std::cout << "[L] BEFORE REMOVING PREV. EDGES ( " << frontier->edges.size() << " )------\n";
            for (int i=0; i<frontier->edges.size(); i++) {
                std::cout << "Edge " << i << " = " << frontier->edges[i] << "\n";
                std::cout << "\tA = " << frontier->edges[i]->a << "\n";
                std::cout << "\tB = " << frontier->edges[i]->b << "\n";
                std::cout << "\tLeft = " << frontier->edges[i]->left << "\n";
                std::cout << "\tRigh = " << frontier->edges[i]->right << "\n";
            }
            std::cout << "\n\n";

            // Remove prev. edges from frontier
            auto pos = std::find(frontier->edges.begin(), frontier->edges.end(), left_edge);
            if (pos != frontier->edges.end()) frontier->edges.erase(pos);
            pos = std::find(frontier->edges.begin(), frontier->edges.end(), left_edge->left);
            if (pos != frontier->edges.end()) frontier->edges.erase(pos);

            // Update left & right
            new_e->left = left_edge->left->left;
            new_e->right = left_edge->right;
            std::cout << "HI!\n";
            left_edge->left->left->right = new_e;
            std::cout << "HI2!\n";
            left_edge->right->left = new_e;
            left_edge->left->left = nullptr;
            left_edge->left->right = nullptr;
            left_edge->left = nullptr;
            left_edge->right = nullptr;
            std::cout << "left_edge = " << left_edge << "\n";
            std::cout << "[L] UPDATED LEFT&RIGHT ( " << frontier->edges.size() << " )------\n";
            for (int i=0; i<frontier->edges.size(); i++) {
                std::cout << "Edge " << i << " = " << frontier->edges[i] << "\n";
                std::cout << "\tA = " << frontier->edges[i]->a << "\n";
                std::cout << "\tB = " << frontier->edges[i]->b << "\n";
                std::cout << "\tLeft = " << frontier->edges[i]->left << "\n";
                std::cout << "\tRigh = " << frontier->edges[i]->right << "\n";
            }
            std::cout << "\n\n";
            /* std::cout << "\nTriangle to legalize: " << t->type << "\n"; */
            /* std::cout << "\t" << t->edges[0] << "->length = " << t->edges[0]->length << "\n"; */
            /* std::cout << "\t" << t->edges[1] << "->length = " << t->edges[1]->length << "\n"; */
            /* std::cout << "\t" << t->edges[2] << "->length = " << t->edges[2]->length << "\n"; */
            t = legalize(t, frontier);
            m_mesh.push_back(t);
            left_edge = new_e;
        } 

        // Right-side walk
        while(right_edge->angleWith(right_edge->right) < M_PI / 2) {
            Triangle* t;
            try { 
                t = new Triangle(right_edge, right_edge->right);
            } catch (const Triangle::IllegalTriangleException &e) {
                break;
            }

            Edge* new_e;
            if (t->edges[0] != right_edge && t->edges[0] != right_edge->right) new_e = t->edges[0];
            else if (t->edges[1] != right_edge && t->edges[1] != right_edge->right) 
                new_e = t->edges[1];
            else new_e = t->edges[2];
            frontier->edges.push_back(new_e);

            std::cout << "right_edge = " << right_edge << "\n";
            std::cout << "[R] BEFORE REMOVING PREV. EDGES ( " << frontier->edges.size() << " )------\n";
            for (int i=0; i<frontier->edges.size(); i++) {
                std::cout << "Edge " << i << " = " << frontier->edges[i] << "\n";
                std::cout << "\tA = " << frontier->edges[i]->a << "\n";
                std::cout << "\tB = " << frontier->edges[i]->b << "\n";
                std::cout << "\tLeft = " << frontier->edges[i]->left << "\n";
                std::cout << "\tRigh = " << frontier->edges[i]->right << "\n";
            }
            std::cout << "\n\n";
            // Remove prev. edges from frontier
            auto pos = std::find(frontier->edges.begin(), frontier->edges.end(), right_edge);
            if (pos != frontier->edges.end()) frontier->edges.erase(pos);
            pos = std::find(frontier->edges.begin(), frontier->edges.end(), right_edge->right);
            if (pos != frontier->edges.end()) frontier->edges.erase(pos);

            // Update left & right
            new_e->left = right_edge->left;
            new_e->right = right_edge->right->right;
            right_edge->left->right = new_e;
            right_edge->right->right->left = new_e;
            right_edge->right->left = nullptr;
            right_edge->right->right = nullptr;
            right_edge->left = nullptr;
            right_edge->right = nullptr;
            std::cout << "right_edge = " << right_edge << "\n";
            std::cout << "[R] AFTER UPDATING LEFT&RIGHT ( " << frontier->edges.size() << " )------\n";
            for (int i=0; i<frontier->edges.size(); i++) {
                std::cout << "Edge " << i << " = " << frontier->edges[i] << "\n";
                std::cout << "\tA = " << frontier->edges[i]->a << "\n";
                std::cout << "\tB = " << frontier->edges[i]->b << "\n";
                std::cout << "\tLeft = " << frontier->edges[i]->left << "\n";
                std::cout << "\tRigh = " << frontier->edges[i]->right << "\n";
            }
            std::cout << "\n\n";

            t = legalize(t, frontier);
            std::cout << "After legalizing\n";
            m_mesh.push_back(t);
            right_edge = new_e;
            std::cout << "Finished loop\n";
        }
       
        // TODO: Remove basins
    }

    std::cout << "\nFINALIZING\n";

    // FINALIZATION 
   
    Edge* initial_edge = frontier->edges[0];
    Edge* edge = initial_edge;
    do {
        if(edge->angleWith(edge->left) < M_PI / 2) {
            Node* left_n = edge->left->a == edge->a ||
                           edge->left->a == edge->b ?
                           edge->left->b : edge->left->a;
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
            frontier->edges.push_back(new_e);

            // Remove prev. edges from frontier
            auto pos = std::find(frontier->edges.begin(), frontier->edges.end(), edge);
            if (pos != frontier->edges.end()) frontier->edges.erase(pos);
            pos = std::find(frontier->edges.begin(), frontier->edges.end(), edge->left);
            if (pos != frontier->edges.end()) frontier->edges.erase(pos);

            // Update left & right
            new_e->left = edge->left->left;
            new_e->right = edge->right;
            edge->left->left->right = new_e;
            edge->right->left = new_e;
            edge->left->left = nullptr;
            edge->left->right = nullptr;
            edge->left = nullptr;
            edge->right = nullptr;

            t = legalize(t, frontier);
            m_mesh.push_back(t);
            edge = new_e;
        } else {
            edge = edge->left;
        }
    } while (edge != initial_edge);

    delete frontier;
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
