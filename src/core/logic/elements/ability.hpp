#ifndef ABILITY_HPP
#define ABILITY_HPP

#include <vector>
#include <chrono>
#include "elem.hpp"
#include "../../physics/movement_manager.hpp"

// Forward declaration
class Bot;

typedef enum AbilityKey {
    q, w, e, r
} AbilityKey;

class Ability: public Elem {
    public:
        time_t getCd();
        void setCd(time_t cd);
        std::chrono::steady_clock::time_point getLastUsed();
        virtual bool cast(Coord target) = 0;
        virtual void update(float ms) = 0;
        virtual void handleBotCollision(Bot* bot) = 0;

    protected:
        Bot* m_bot;
        time_t m_cd;
        std::chrono::steady_clock::time_point m_last_used;
        Ability();
        Ability(ElemType type, bool alive, ConvexPolygon* shape, Coord center, Team team,
                Bot* bot, time_t cd, int bounding_sphere_radius);
};

#endif
