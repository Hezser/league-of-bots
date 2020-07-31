/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "polygon.hpp"

using namespace adamant::graphics;
using namespace adamant::graphics::elements;

Polygon::Polygon(ShapeType subtype): Shape(subtype) {}

Polygon::Polygon(std::vector<Node*> nodes, std::vector<Edge*> edges): Shape(polygon) {
    this->edges = edges;
    this->nodes = nodes;
    std::vector<Coord> coords;
    for (Edge* e : edges) {
        e->shape_ptrs.push_back(this);
    }
    for (Node* n : nodes) {
        coords.push_back(n->coord);
    }
    m_center = findCenter(coords);
    // TODO: SFML does not supprot concave shapes, so we can't create the drawable
}

Polygon::~Polygon() {
    for (Edge* e : edges) {
        if (e->shape_ptrs.size() == 1) {
            // Delete the edge if it only belongs to this shape
            delete e;
        } else {
            // Remove shape from each edge's shape_ptrs otherwise
            auto pos = std::find(e->shape_ptrs.begin(), e->shape_ptrs.end(), this);
            if (pos != e->shape_ptrs.end()) e->shape_ptrs.erase(pos);
        }
    }
}

void Polygon::setCenter(Coord center) {
    Coord prev_center = m_center;
    m_center = center;
    Coord diff = {m_center.x - prev_center.x, m_center.y - prev_center.y};
    for (Node* node : nodes) {
        node->coord.x += diff.x;    
        node->coord.y += diff.y;    
    }
}

Coord Polygon::findCenter(std::vector<Coord> coords) {
    if (coords.size() == 0) return {0,0};
    Coord max = {0, 0};  // Assuming no negative values, as coordinates are always positive
    Coord min = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    for (auto coord : coords) {
        if (coord.x > max.x) max.x = coord.x;
        if (coord.x < min.x) min.x = coord.x;
        if (coord.y > max.y) max.y = coord.y;
        if (coord.y < min.y) min.y = coord.y;
    }
    return {(max.x + min.x) / 2, (max.y + min.y) / 2};
}

void Polygon::defineNeighboursFromCenter(Coord origin) {
    // Calculate r and theta with respect to the center
    for (Node* n : nodes) {
        n->setOrigin(m_center);
    }

    // Define left & right
    for (Edge* e : edges) {
        for (Edge* g : edges) {
            if (e != g) {
                if (e->hasAtLeft(g)) {
                    e->left = g;
                    g->right = e;
                } else {
                    e->right = g;
                    g->left = e;
                }
            }
        }
    }

    // Restore original center
    for (Node* n : nodes) {
        n->setOrigin(origin);
    }
}

sf::Shape* Polygon::getDrawable() {
    unsigned int min_x = std::numeric_limits<unsigned int>::max();
    unsigned int min_y = std::numeric_limits<unsigned int>::max();
    for (Node* n : nodes) {
        if (n->coord.x < min_x) min_x = n->coord.x;
        if (n->coord.y < min_y) min_y = n->coord.y;
    }
    drawable->setPosition(min_x, min_y);
    return drawable;
}

const char* Polygon::InsufficientNodesException::what() const throw() {
    return "Less than 3 nodes were given.";
}

