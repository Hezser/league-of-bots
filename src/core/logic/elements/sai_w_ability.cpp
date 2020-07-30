#include "sai_w_ability.hpp"
#include "bot.hpp"

SaiWAbility::SaiWAbility(Bot* bot): Ability(ability_t, false,
        new ConvexPolygon({{0,1}, {1,0}, {1,1}, {0,1}}), {0,0}, bot->getTeam(), bot, 2000, 0) {}

bool SaiWAbility::cast(Coord target) {
    return true; 
}

void SaiWAbility::update(float ms) {

}

void SaiWAbility::handleBotCollision(Bot* bot) {

}

