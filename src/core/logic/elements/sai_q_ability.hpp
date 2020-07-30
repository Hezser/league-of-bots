#ifndef SAI_Q_ABILITY_HPP
#define SAI_Q_ABILITY_HPP

#include "ability.hpp"

class SaiQAbility: public Ability {        
    public: 
        SaiQAbility(Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;

    private:
        MovementManager* m_movement_manager;
};

#endif
