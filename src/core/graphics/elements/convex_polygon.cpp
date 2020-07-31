/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "convex_polygon.hpp"
#include <algorithm>

using namespace adamant::graphics::elements;

ConvexPolygon::ConvexPolygon(ShapeType subtype): Polygon(subtype) {}

ConvexPolygon::ConvexPolygon(std::vector<Coord> coords): Polygon(convex_polygon) {
    if (coords.size() < 3) throw InsufficientNodesException();

    m_center = findCenter(coords);

    // Create nodes
    nodes = createNodes(coords);
    std::sort(nodes.begin(), nodes.end(), Node::ThetaComparator());

    // Create edges
    for (int i=0; i<(int)nodes.size()-1; i++) {
        Edge* e;
        try {
            e = new Edge(nodes[i], nodes[i+1], this);
        } catch (Edge::ExistingEdgeException &ex) {
            e = ex.getExistingEdge();
            e->shape_ptrs.push_back(this);
        }
        edges.push_back(e);
    }
    if (nodes.size() > 1) {
        Edge* e;
        try {
            e = new Edge(nodes[nodes.size()-1], nodes[0], this);
        } catch (Edge::ExistingEdgeException &ex) {
            e = ex.getExistingEdge();
            e->shape_ptrs.push_back(this);
        }
        edges.push_back(e);
    }

    // Set edges' left and right
    for (int i=1; i<(int)edges.size()-1; i++) {
        edges[i]->left = edges[i+1];
        edges[i]->right = edges[i-1];
    }
    edges[0]->left = edges[1];
    edges[0]->right = edges[(int)edges.size()-1];
    edges[(int)edges.size()-1]->left = edges[0];
    edges[(int)edges.size()-1]->right = edges[(int)edges.size()-2];

    constructDrawable(coords);
}

void ConvexPolygon::constructDrawable(std::vector<Coord> coords) {
    sf::ConvexShape* d = new sf::ConvexShape();
    d->setPointCount(coords.size());
    for (auto i=0; i<coords.size(); i++) {
        d->setPoint(i, sf::Vector2f(coords[i].x, coords[i].y));
    }
    drawable = d;
}

std::vector<Node*> ConvexPolygon::createNodes(std::vector<Coord> coords) {
    std::vector<Node*> nodes;
    for (Coord c : coords) {
        nodes.push_back(new Node({c.x, c.y}, m_center));
    }
    return nodes;
}

