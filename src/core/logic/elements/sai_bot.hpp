#ifndef SAI_BOT_HPP
#define SAI_BOT_HPP

#include "bot.hpp"

class SaiBot: public Bot {
    public:
        SaiBot(Team team, Coord start);
        void update(float ms) override;
};

#endif
