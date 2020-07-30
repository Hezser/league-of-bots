#include "sai_q_ability.hpp"
#include "bot.hpp"
#include "../../physics/linear_move.hpp"

SaiQAbility::SaiQAbility(Bot* bot): Ability(ability_t,
        false, new ConvexPolygon({{0,0}, {0,5}, {5,5}, {5,0}}), {0,0}, bot->getTeam(), bot,
        2000, 5), m_movement_manager{new MovementManager(this, 3.5f)} {
    m_shape->getDrawable()->setFillColor(sf::Color::Yellow);
    if (m_team == white_team) m_shape->getDrawable()->setOutlineColor(sf::Color::Green);
    else m_shape->getDrawable()->setOutlineColor(sf::Color::Red);
    m_shape->getDrawable()->setOutlineThickness(1);
}

bool SaiQAbility::cast(Coord target) {
    m_bot->mutex.lock();
    Coord start = m_bot->getCenter();
    m_bot->mutex.unlock();
    m_shape->setCenter(start);
    Move* move = new LinearMove(start, target, own_ability);
    mutex.lock();
    m_movement_manager->request(move);
    m_alive = true;
    mutex.unlock();
    return true;
}

void SaiQAbility::update(float ms) {
    if (!m_movement_manager->update(ms)) {
        m_alive = false;
    }
    // More stuff...
}

void SaiQAbility::handleBotCollision(Bot* bot) {
    if (bot->getTeam() != m_team) {
        bot->kill();
    }
}

