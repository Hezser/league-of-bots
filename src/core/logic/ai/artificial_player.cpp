#include <chrono>
#include "artificial_player.hpp"

/* class ArtificialPlayer */

ArtificialPlayer::ArtificialPlayer(Bot* bot, float update_interval, ArtificialMovementPolicy
        movement_policy, ArtificialAimingPolicy aiming_policy): m_bot{bot}, 
        m_update_interval{update_interval}, m_movement_policy{movement_policy}, 
        m_aiming_policy{aiming_policy}, m_last_played{std::chrono::steady_clock::now()} {}

float ArtificialPlayer::getUpdateInterval() {
    return m_update_interval;
}

void ArtificialPlayer::setUpdateInterval(float update_interval) {
    m_update_interval = update_interval;
}

ArtificialMovementPolicy ArtificialPlayer::getMovementPolicy() {
    return m_movement_policy;
}

void ArtificialPlayer::setMovementPolicy(ArtificialMovementPolicy movement_policy) {
    m_movement_policy = movement_policy;
}

ArtificialAimingPolicy ArtificialPlayer::getAimingPolicy() {
    return m_aiming_policy;
}

void ArtificialPlayer::setAimingPolicy(ArtificialAimingPolicy aiming_policy) {
    m_aiming_policy = aiming_policy;
}

void ArtificialPlayer::play() {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<float, std::milli> elapsed = now - m_last_played;
    if (elapsed.count() >= m_update_interval) {
        // Movement
        if (m_movement_policy == random_movement) {
            moveBotRandomly();
        }
        // TODO: Aiming
        m_last_played = now;
    }
}

void ArtificialPlayer::moveBotRandomly() {
    int x = std::rand() % 1000;  // TODO: Adjust to screen size (not arbitrary 1000)
    int y = std::rand() % 1000;  // TODO: Adjust to screen size (not arbitrary 1000)
    m_bot->moveTowards({x, y});
}
