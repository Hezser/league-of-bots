/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef ELEM_HPP
#define ELEM_HPP

#include "../../graphics/coord.hpp"
#include "../../graphics/elements/convex_polygon.hpp"
#include <chrono>
#include <vector>
#include <mutex>
#include <cmath>

namespace adamant {
namespace logic {
namespace elements {

typedef enum ElemType {  
    bot_t = 0, ability_t = 1, terrain_t = 2
} ElemType;  
  
typedef enum Team {  
    neutral_team = 0, white_team = 1, black_team = 2  
} Team;

class Elem {
    public:
        mutable std::mutex mutex;
        ElemType getType();
        bool isAlive();
        graphics::elements::ConvexPolygon* getShape();
        void setShape(graphics::elements::ConvexPolygon* shape);
        graphics::Coord getCenter();
        void setCenter(graphics::Coord center);
        Team getTeam();
        int getBoundingSphereRadius();
        void setBoundingSphereRadius(int radius);
        void kill();
        virtual void update(float ms) = 0;

    protected:
        ElemType m_type;
        bool m_alive;
        graphics::elements::ConvexPolygon* m_shape;
        Team m_team;
        int m_bounding_sphere_radius;
        Elem();
        Elem(ElemType type, bool alive, graphics::elements::ConvexPolygon* shape,
                graphics::Coord center, Team team, int boundingSphereRadius);
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
