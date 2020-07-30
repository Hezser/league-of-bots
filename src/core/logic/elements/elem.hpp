/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef ELEM_HPP
#define ELEM_HPP

#include "../../graphics/elements/coord.hpp"
#include "../../graphics/elements/convex_polygon.hpp"
#include <chrono>
#include <vector>
#include <mutex>
#include <cmath>

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
        ConvexPolygon* getShape();
        void setShape(ConvexPolygon* shape);
        Coord getCenter();
        void setCenter(Coord center);
        Team getTeam();
        int getBoundingSphereRadius();
        void setBoundingSphereRadius(int radius);
        void kill();
        virtual void update(float ms) = 0;

    protected:
        ElemType m_type;
        bool m_alive;
        ConvexPolygon* m_shape;
        Team m_team;
        int m_bounding_sphere_radius;
        Elem();
        Elem(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team,
                int boundingSphereRadius);
};

#endif
