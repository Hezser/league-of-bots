#ifndef SAI_R_ABILITY_HPP
#define SAI_R_ABILITY_HPP

#include "ability.hpp"

class SaiRAbility: public Ability {        
    public:                        
        SaiRAbility(class Bot* bot);
        bool cast(Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;
};

#endif
