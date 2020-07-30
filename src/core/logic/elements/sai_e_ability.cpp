#include "sai_e_ability.hpp"
#include "bot.hpp"

SaiEAbility::SaiEAbility(Bot* bot): Ability(ability_t, false,
        new ConvexPolygon({{0,1}, {1,0}, {1,1}, {0,1}}), {0,0}, bot->getTeam(), bot, 2000, 0) {}

bool SaiEAbility::cast(Coord target) {
    return true; 
}

void SaiEAbility::update(float ms) {
    
}

void SaiEAbility::handleBotCollision(Bot* bot) {

}
