/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef SAI_R_ABILITY_HPP
#define SAI_R_ABILITY_HPP

#include "ability.hpp"

namespace adamant {
namespace logic {
namespace elements {

class SaiRAbility: public Ability {        
    public:                        
        SaiRAbility(class Bot* bot);
        bool cast(graphics::Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
