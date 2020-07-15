#ifndef ABILITY_HPP
#define ABILITY_HPP

#include <vector>
#include <chrono>
#include "elem.hpp"
#include "../../physics/movement.hpp"

// Forward-declared
class Bot;

typedef enum AbilityKey {
    q, w, e, r
} AbilityKey;

class Ability: public Elem {
    public:
        time_t getCd();
        void setCd(time_t cd);
        std::chrono::steady_clock::time_point getLastUsed();
        virtual bool cast(Coord target) = 0;
        virtual void update(float ms) = 0;
        virtual void handleBotCollision(Bot* bot) = 0;

    protected:
        Bot* m_bot;
        time_t m_cd;
        std::chrono::steady_clock::time_point m_last_used;
        Ability();
        Ability(ElemType type, bool alive, Shape* shape, Coord center, Team team,
                Bot* bot, time_t cd, int bounding_sphere_radius);
};

class SaiQAbility: public Ability {        
    public: 
        SaiQAbility(Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;

    private:
        MovementManager* m_movement_manager;
};

class SaiWAbility: public Ability {        
    public:                        
        SaiWAbility(class Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;
};

class SaiEAbility: public Ability {        
    public:                        
        SaiEAbility(class Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;
};

class SaiRAbility: public Ability {        
    public:                        
        SaiRAbility(class Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;
};

#endif
