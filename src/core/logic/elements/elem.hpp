#ifndef ELEM_HPP
#define ELEM_HPP

#include <chrono>
#include <vector>
#include <mutex>
#include <cmath>

typedef enum ElemType {  
    bot_t,  
    ability_t  
} ElemType;  
  
typedef enum Team {  
    neutral_team,  
    white_team,  
    black_team  
} Team;

class Elem {
    public:
        mutable std::mutex mutex;
        ElemType getType();
        bool isAlive();
        std::vector<int> getCoord();
        void setCoord(std::vector<int> coord);
        Team getTeam();
        int getBoundingSphereRadius();
        void setBoundingSphereRadius(int radius);
        void kill();
        virtual void update(float ms) = 0;

    protected:
        ElemType m_type;
        bool m_alive;
        std::vector<int> m_coord;
        Team m_team;
        int m_bounding_sphere_radius;
        Elem(ElemType type, bool alive, std::vector<int> coord, Team team, int boundingSphereRadius);
};

#endif
