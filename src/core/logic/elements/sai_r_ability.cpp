#include "sai_r_ability.hpp"
#include "bot.hpp"

SaiRAbility::SaiRAbility(Bot* bot): Ability(ability_t, false,
        new ConvexPolygon({{0,1}, {1,0}, {1,1}, {0,1}}), {0,0}, bot->getTeam(), bot, 2000, 0) {}

bool SaiRAbility::cast(Coord target) {
    return true; 
}

void SaiRAbility::update(float ms) {

}

void SaiRAbility::handleBotCollision(Bot* bot) {

}
