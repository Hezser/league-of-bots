#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <algorithm>
#include <cstdlib>
#include <exception>
#include <thread>
#include <iostream>
#include <chrono>
#include <mutex>
#include <vector>
#include <cmath>
#include <atomic>

// Flag telling the movement thread to continue or stop
std::atomic<bool> stop_movement_flag{false};

const int MAX_QABILITY_RANGE = 100;
const int MAX_EABILITY_RANGE = 100;
const float QABILITY_VELOCITY = 3.0f;
const float BOT_VELOCITY = 1.0f;

enum ElemType {
    bot_type = 0, qAbility_type = 1
};

enum Team {
    red_team = 0, blue_team = 1, neutral = 2
};

class Elem {
    public:
        ElemType type;
        bool alive;
        Team team;
};

class Bot : public Elem {
    public:
        mutable std::mutex mutex;
        int x;
        int y;
        const time_t qAbilityCd = std::chrono::milliseconds(1000).count();
        const time_t eAbilityCd = std::chrono::milliseconds(3000).count();
        std::chrono::steady_clock::time_point last_qAbility;
        std::chrono::steady_clock::time_point last_eAbility;

        Bot(Team team, int x, int y) {
            Elem::type = bot_type;
            Elem::alive = true;
            Elem::team = team;
            this->x = x;
            this->y = y;
        }
};

class QAbility : public Elem {
    public:
        mutable std::mutex mutex;
        int x;
        int y;

        QAbility(Team team, int x, int y) {
            Elem::type = qAbility_type;
            Elem::alive = true;
            Elem::team = team;
            this->x = x;
            this->y = y;
        }
};

/* TODO: Need a way to adapt to external changes to the position of the bot (other abilities, etc.)
   Either we re-evaluate the target vector on every iteration of the loop (which will be costly)
   or we create a system of signals such that any other event that changes the position of the bot 
   sends a signal to the moveBot thread to adjust */
void moveBot(Bot* bot, int x, int y) {
    // We convert the vector (x, y) to a basis where (bot.x, bot.y) is the origin
    std::vector<float> alphaTarget = {(float)x - bot->x, (float)y - bot->y};
    // We normalize the alpha vector
    int d = std::round(sqrt(pow(alphaTarget[0], 2) + pow(alphaTarget[1], 2)));
    std::vector<float> alphaUnitTarget = {alphaTarget[0] / d, alphaTarget[1] / d};
    // We move one unit of space every unit of time towards the target
    int prev_x = 0, prev_y = 0, tmp_x = 0, tmp_y = 0;
    for (int i=1; i<=d; i++) {
        if (stop_movement_flag) {
            break;
        }
        tmp_x = (int) (BOT_VELOCITY * i * alphaUnitTarget[0]);
        tmp_y = (int) (BOT_VELOCITY * i * alphaUnitTarget[1]);
        bot->mutex.lock();
        bot->x += tmp_x - prev_x;
        bot->y += tmp_y - prev_y;
        bot->mutex.unlock();
        prev_x = tmp_x;
        prev_y = tmp_y;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

void useQAbility(QAbility* qAbility, Bot* bot, int x, int y) {
    // The start position vector
    std::vector<int> start = {bot->x, bot->y};
    // We convert the vector (x, y) to a basis where (bot.x, bot.y) is the origin
    std::vector<float> alphaTarget = {(float)x - bot->x, (float)y - bot->y};
    // We normalize the alpha vector
    int d = sqrt(pow(alphaTarget[0], 2) + pow(alphaTarget[1], 2));
    std::vector<float> alphaUnitTarget = {alphaTarget[0] / d, alphaTarget[1] / d};
    // We move one unit of space every unit of time towards the target
    for (auto i=1; i<=MAX_QABILITY_RANGE; i++) {
        qAbility->mutex.lock();
        qAbility->x = (int) (QABILITY_VELOCITY * i * alphaUnitTarget[0]) + start[0];
        qAbility->y = (int) (QABILITY_VELOCITY * i * alphaUnitTarget[1]) + start[1];
        qAbility->mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    qAbility->alive = false;
}

void useEAbility(Bot* bot, int x, int y) {
    // We convert the vector (x, y) to a basis where (bot.x, bot.y) is the origin
    std::vector<float> alphaTarget = {(float)x - bot->x, (float)y - bot->y};
    // We normalize the alpha vector
    int d = sqrt(pow(alphaTarget[0], 2) + pow(alphaTarget[1], 2));
    std::vector<float> alphaUnitTarget = {alphaTarget[0] / d, alphaTarget[1] / d};
    // We choose the right distance
    d = std::min(d, MAX_EABILITY_RANGE);
    // We move the bot
    bot->mutex.lock();
    bot->x += d * alphaUnitTarget[0];
    bot->y += d * alphaUnitTarget[1];
    bot->mutex.unlock();
}

int main() {
    
    // Graphical elements that need to be displayed on the next frame
    std::vector<Elem*> elems;
    Bot* bot = new Bot(red_team, 0, 0);
    elems.push_back(bot);
    std::thread botMovement;

    sf::RenderWindow window(sf::VideoMode(200, 200), "It works!");

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    if (botMovement.joinable()) {
                        stop_movement_flag = true;
                        botMovement.join();
                        stop_movement_flag = false;
                    }
                    botMovement = std::thread(moveBot, bot, event.mouseButton.x, event.mouseButton.y);
                }
            } else if (event.type == sf::Event::KeyPressed) {
                std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
                if (event.key.code == sf::Keyboard::Q) {
                    auto target = sf::Mouse::getPosition(window);
                    QAbility* qAbility = new QAbility(bot->team, bot->x, bot->y);
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - bot->last_qAbility).count() >= bot->qAbilityCd || bot->last_qAbility.time_since_epoch().count() == 0) {
                        bot->last_qAbility = now;
                        std::thread qAbilityThread(useQAbility, qAbility, bot, target.x, target.y);
                        qAbilityThread.detach();
                        elems.push_back(qAbility);
                    } else {
                        delete qAbility;
                    }
                } else if (event.key.code == sf::Keyboard::E) {
                    auto target = sf::Mouse::getPosition(window);
                    if (std::chrono::duration_cast<std::chrono::milliseconds>(now - bot->last_eAbility).count() >= bot->eAbilityCd || bot->last_eAbility.time_since_epoch().count() == 0) {
                        bot->last_eAbility = now;
                        std::thread eAbilityThread(useEAbility, bot, target.x, target.y);
                        eAbilityThread.detach();
                    }
                }
            }
        }

        // Clear previous frame
        window.clear();

        for (auto i=0; i<elems.size(); i++) {
            // Garbage collector
            if (!elems[i]->alive) {
                delete elems[i];
                elems.erase(elems.begin() + i);
                continue;
            }
            // Draw element
            switch (elems[i]->type) {
                case bot_type: {
                    Bot* bot = (Bot*) elems[i];
                    sf::RectangleShape botMesh;
                    botMesh.setSize(sf::Vector2f(20, 20));
                    botMesh.setOutlineColor(sf::Color::Red);
                    botMesh.setOutlineThickness(2);
                    botMesh.setPosition(bot->x, bot->y);
                    window.draw(botMesh);
                    break;
                }
                case qAbility_type: {
                    QAbility* qAbility = (QAbility*) elems[i];
                    sf::RectangleShape qAbilityMesh;
                    qAbilityMesh.setSize(sf::Vector2f(5, 5));
                    qAbilityMesh.setOutlineColor(sf::Color::Green);
                    qAbilityMesh.setOutlineThickness(1);
                    qAbilityMesh.setPosition(qAbility->x, qAbility->y);
                    window.draw(qAbilityMesh);
                    break;
                }
                default:
                    break;
            }
        }

        // Debuffer the frame
        window.display();
    }

    return 0;
}
