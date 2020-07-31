/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef PATH_FINDER_HPP
#define PATH_FINDER_HPP

#include "move.hpp"
#include "../graphics/coord.hpp"
#include "../graphics/nav_mesh.hpp"
#include <vector>

namespace adamant {
namespace physics {
namespace movement {

typedef std::vector<Move> Path;

class PathFinder {
    public:
        PathFinder(graphics::NavMesh);
        Path findPath(graphics::Coord start, graphics::Coord target);

    private:
        graphics::NavMesh m_nav_mesh;
};

}  // namespace movement
}  // namespace physics
}  // namespace adamant

#endif
