#ifndef HULL_HPP
#define HULL_HPP

#include "coord.hpp"
#include "node.hpp"
#include "edge.hpp"
#include <vector>

class Hull {
    public:
        Coord origin;
        std::vector<Edge*> edges;
        Hull(Coord origin);
        Edge* getIntersectingEdge(Node* node);

    private:
        Hull();
} ;

#endif
