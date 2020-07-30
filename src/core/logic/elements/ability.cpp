#include <chrono>
#include "ability.hpp"
#include "elem.hpp"
#include "bot.hpp"

Ability::Ability(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team,
        Bot* bot, time_t cd, int bounding_sphere_radius): Elem(type, alive, shape, center,
        team, bounding_sphere_radius), m_bot{bot}, m_cd{cd},
        m_last_used{std::chrono::steady_clock::time_point(std::chrono::seconds(0))} {}

time_t Ability::getCd() {
    return m_cd;
}

void Ability::setCd(time_t cd) {
    m_cd = cd;
}

std::chrono::steady_clock::time_point Ability::getLastUsed() {
    return m_last_used;
}
