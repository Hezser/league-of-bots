/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef SAI_BOT_HPP
#define SAI_BOT_HPP

#include "bot.hpp"

namespace adamant {
namespace logic {
namespace elements {

class SaiBot: public Bot {
    public:
        SaiBot(Team team, graphics::Coord start);
        void update(float ms) override;
};

}  // namespace elements
}  // namespace logic
}  // namespace adamant

#endif
