/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "hull.hpp"
#include <cmath>
#include <algorithm>

Hull::Hull(Coord origin) {
    this->origin = origin;
}

/* If there are >3 edges being collinear and a node intersects with them, the node needs to
 * get the closest edge to it (furtherst away from origin), which guarantees that the node can
 * make a non-collinear triangle with the left or right edge of the intersector */
Edge* Hull::getIntersectingEdge(Node* node) {
    // Empty hull
    if (edges.size() == 0) return nullptr;

    // The center of the hull is inside the hull itself
    int j = -1;
    for (int i=0; i<edges.size(); i++) {
        Edge* e = edges[i];
        /* The node's angle is within the edge's angles (it intersects with it),
         * accounting for the exception of when e passes through theta=0 */
        if ((e->a->theta > node->theta && e->b->theta < node->theta) || 
             (e->b->theta > node->theta && e->a->theta < node->theta) ||
             (e->a->theta > node->theta && e->b->theta > node->theta && 
                    std::abs(e->a->theta - e->b->theta) >= M_PI) ||
             (e->a->theta == node->theta)) {
            float e_to_node = e->shortestDistanceTo(node->coord);
            // Pick the closest intersector to the node
            if (j == -1 || e->shortestDistanceTo(node->coord) <
                edges[j]->shortestDistanceTo(node->coord)) j = i;
        }
    }

    // We did not find a suitable intersector, so we choose the closest legal edge to the node
    if (j == -1) {
        std::vector<Edge*> es = edges;
        std::sort(es.begin(), es.end(), [&](Edge* lhs, Edge* rhs) {
            return lhs->shortestDistanceTo(node->coord) < rhs->shortestDistanceTo(node->coord);
        });
        std::vector<Edge*> closer;
        for (int i=0; i<es.size(); i++) {
            if (es[i]->a == node || es[i]->b == node) continue;
            Edge* e;
            Edge* g;
            bool e_exists = false;
            bool g_exists = false;
            try {
                e = new Edge(node, es[i]->a);
            } catch (Edge::ExistingEdgeException &ex) {
                e = ex.getExistingEdge();
                e_exists = true;
            }
            try {
                g = new Edge(node, es[i]->b);
            } catch (Edge::ExistingEdgeException &ex) {
                g = ex.getExistingEdge();
                g_exists = true;
            }
            // Check for legality
            bool legal = !es[i]->isCollinearWithNode(node) && !e->intersectsWith(closer) &&
                         !g->intersectsWith(closer);
            if (!e_exists) delete e;
            if (!g_exists) delete g;
            if (legal) {
                auto pos = std::find(edges.begin(), edges.end(), es[i]);
                if (pos != edges.end()) j = std::distance(edges.begin(), pos);
                break;
            }
            closer.push_back(es[i]);
        }
    }

    if (j == -1) return nullptr; 

    return edges[j];
}
