#ifndef BOT_HPP
#define BOT_HPP

#include <chrono>
#include <vector>
#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/movement.hpp"

class Bot: public Elem {
    public:
        void moveTowards(Coord target);
        void moveTo(Coord target);
        Ability* useAbility(AbilityKey key, Coord target);
        virtual void update(float ms) = 0;

    protected:
        Bot(ElemType type, bool alive, Coord coord, Team team,
                int bounding_sphere_radius, MovementManager* movement_manager);
        std::vector<Ability*> m_abilities;
        MovementManager* m_movement_manager;
};

class SaiBot: public Bot {
    public:
        SaiBot(Team team, Coord start);
        void update(float ms) override;
};

#endif
