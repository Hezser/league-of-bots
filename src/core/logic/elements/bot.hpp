#ifndef BOT_HPP
#define BOT_HPP

#include <chrono>
#include <vector>
#include "elem.hpp"
#include "ability.hpp"
#include "../../physics/movement.hpp"

class Bot: public Elem {
    public:
        void moveTowards(std::vector<int> target);
        void moveTo(std::vector<int> target);
        Ability* useAbility(AbilityKey key, std::vector<int> target);
        virtual void update(float ms) = 0;

    protected:
        Bot(ElemType type, bool alive, std::vector<int> coord, Team team,
                int bounding_sphere_radius, std::vector<Ability*> abilities, 
                MovementManager* movement_manager);
        std::vector<Ability*> m_abilities;
        MovementManager* m_movement_manager;
};

class SaiBot: public Bot {
    public:
        SaiBot(Team team, std::vector<int> start);
        void update(float ms) override;
};

#endif
