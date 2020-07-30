/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef SAI_BOT_HPP
#define SAI_BOT_HPP

#include "bot.hpp"

class SaiBot: public Bot {
    public:
        SaiBot(Team team, Coord start);
        void update(float ms) override;
};

#endif
