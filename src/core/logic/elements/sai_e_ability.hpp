#ifndef SAI_E_ABILITY_HPP
#define SAI_E_ABILITY_HPP

#include "ability.hpp"

class SaiEAbility: public Ability {
    public:                        
        SaiEAbility(class Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;
};

#endif
