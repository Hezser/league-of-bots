/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include "sai_bot.hpp"
#include "sai_q_ability.hpp"
#include "sai_w_ability.hpp"
#include "sai_e_ability.hpp"
#include "sai_r_ability.hpp"

SaiBot::SaiBot(Team team, Coord start): Bot(bot_t, true, new ConvexPolygon({{0,0}, {0,50},
            {50,50}, {50, 0}}), start, team, 14, new MovementManager(this, 2.5f)) {
    /* We initialize abilities after the bot is fully initialized so that the Ability
     * constructor can use the bot's members */
    m_shape->getDrawable()->setFillColor(sf::Color::White);
    if (m_team == white_team) m_shape->getDrawable()->setOutlineColor(sf::Color::Green);
    else m_shape->getDrawable()->setOutlineColor(sf::Color::Red);
    m_shape->getDrawable()->setOutlineThickness(2);
    m_abilities = {new SaiQAbility(this), new SaiWAbility(this), new SaiEAbility(this), 
                   new SaiRAbility(this)};
}

void SaiBot::update(float ms) {
    m_movement_manager->update(ms);
    // In the future, health, mana, experience, etc. will also need to be updated here
}
