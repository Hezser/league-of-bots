#ifndef PATH_FINDER_HPP
#define PATH_FINDER_HPP

#include <vector>
#include "nav_mesh.hpp"
#include "movement.hpp"

typedef std::vector<Move> Path;

class PathFinder {
    public:
        PathFinder(NavMesh);
        Path findPath(Coord start, Coord target);

    private:
        NavMesh m_nav_mesh;
};

#endif
