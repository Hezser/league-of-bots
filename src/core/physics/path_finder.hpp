#ifndef PATH_FINDER_HPP
#define PATH_FINDER_HPP

#include "move.hpp"
#include "../graphics/elements/coord.hpp"
#include "../graphics/nav_mesh.hpp"
#include <vector>

typedef std::vector<Move> Path;

class PathFinder {
    public:
        PathFinder(NavMesh);
        Path findPath(Coord start, Coord target);

    private:
        NavMesh m_nav_mesh;
};

#endif
