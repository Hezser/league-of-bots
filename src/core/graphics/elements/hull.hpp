/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef HULL_HPP
#define HULL_HPP

#include "../coord.hpp"
#include "node.hpp"
#include "edge.hpp"
#include <vector>

namespace adamant {
namespace graphics {
namespace elements {

class Hull {
    public:
        Coord origin;
        std::vector<Edge*> edges;
        Hull(Coord origin);
        Edge* getIntersectingEdge(Node* node);

    private:
        Hull();
};

}  // namespace elements
}  // namespace graphics
}  // namespace adamant

#endif
