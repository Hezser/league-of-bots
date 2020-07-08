#ifndef ELEM_HPP
#define ELEM_HPP

#include <chrono>
#include <vector>
#include <mutex>
#include <cmath>
#include "../../physics/shape.hpp"

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
        Coord getCoord();
        void setCoord(Coord coord);
        Team getTeam();
        int getBoundingSphereRadius();
        void setBoundingSphereRadius(int radius);
        void kill();
        virtual void update(float ms) = 0;

    protected:
        ElemType m_type;
        bool m_alive;
        Coord m_coord;
        Team m_team;
        int m_bounding_sphere_radius;
        Elem(ElemType type, bool alive, Coord coord, Team team, int boundingSphereRadius);
};

#endif
