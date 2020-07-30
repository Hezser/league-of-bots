#ifndef BOT_HPP
#define BOT_HPP

#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/movement_manager.hpp"
#include <chrono>
#include <vector>

class Bot: public Elem {
    public:
        void moveTowards(Coord target);
        void moveTo(Coord target);
        Ability* useAbility(AbilityKey key, Coord target);
        virtual void update(float ms) = 0;

    protected:
        Bot();
        Bot(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team,
                int bounding_sphere_radius, MovementManager* movement_manager);
        std::vector<Ability*> m_abilities;
        MovementManager* m_movement_manager;
};

#endif
