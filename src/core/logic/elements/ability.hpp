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
        virtual bool cast(std::vector<int> target) = 0;
        virtual void update(double ms) = 0;

    protected:
        Bot* m_bot;
        time_t m_cd;
        std::chrono::steady_clock::time_point m_last_used;
        Ability(ElemType type, bool alive, std::vector<int> coord, Team team,
                Bot* bot, time_t cd);
};

class SaiQAbility: public Ability {        
    public: 
        SaiQAbility(Bot* bot);
        bool cast(std::vector<int> target) override;
        void update(double ms) override;

    private:
        MovementManager* m_movement_manager;
};

class SaiWAbility: public Ability {        
    public:                        
        SaiWAbility(class Bot* bot);
        bool cast(std::vector<int> target) override;
        void update(double ms) override;
};

class SaiEAbility: public Ability {        
    public:                        
        SaiEAbility(class Bot* bot);
        bool cast(std::vector<int> target) override;
        void update(double ms) override;
};

class SaiRAbility: public Ability {        
    public:                        
        SaiRAbility(class Bot* bot);
        bool cast(std::vector<int> target) override;
        void update(double ms) override;
};

#endif
