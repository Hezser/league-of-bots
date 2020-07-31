/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef SAI_Q_ABILITY_HPP
#define SAI_Q_ABILITY_HPP

#include "ability.hpp"

namespace adamant {
namespace logic {
namespace elements {

class SaiQAbility: public Ability {        
    public: 
        SaiQAbility(Bot* bot);
        bool cast(graphics::Coord target) override;
        void update(float ms) override;
        void handleBotCollision(Bot* bot) override;

    private:
        physics::movement::MovementManager* m_movement_manager;
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
