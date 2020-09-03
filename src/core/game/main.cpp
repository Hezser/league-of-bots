/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include <SFML/Graphics/CircleShape.hpp>
#include <chrono>
#include <vector>
#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "../logic/elements/elem.hpp"
#include "../logic/elements/sai_bot.hpp"
#include "../logic/elements/ability.hpp"
#include "../logic/elements/terrain.hpp"
#include "../physics/collision_detection_system.hpp"
#include "../physics/collision_resolution_system.hpp"
#include "../logic/ai/artificial_player.hpp"

using namespace adamant::logic::ai;
using namespace adamant::logic::elements;
using namespace adamant::physics::collision;
using namespace adamant::graphics::elements;

int main() {
    // TODO: Use tai_clock when C++20 is released; system_clock can be altered by changing the time of the system
    std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
    std::vector<Elem*> elems;
    // Player's bot
    SaiBot* sai = new SaiBot(white_team, {1000, 500});
    elems.push_back(sai);
    // Enemy AI-controlled bots
    std::vector<ArtificialPlayer*> ais;
    for (int i=0; i<5; i++) {
        SaiBot* new_bot = new SaiBot(black_team, {i*100, 100});
        elems.push_back(new_bot);
        ais.push_back(new ArtificialPlayer(new_bot, 600, random_movement, random_aiming));
    }
    // Obstacles
    for (auto i=0; i<6; i++) {
        Terrain* obstacle = new Terrain(new ConvexPolygon({{0,0}, {0,50}, {50,50},
                    {50,0}}), {std::rand() % 2000, std::rand() % 2000}, 50);
        elems.push_back(obstacle);
    }

    sf::RenderWindow window(sf::VideoMode(700, 700), "Loading...");

    while (window.isOpen()) {
        // Get and process input
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                // Garbage collector
                for (auto elem : elems) {
                    delete elem;
                }
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Right) {
                    sai->moveTowards({event.mouseButton.x, event.mouseButton.y});
                }
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    auto target = sf::Mouse::getPosition(window);
                    Ability* casted_ability = sai->useAbility(q, {target.x, target.y});
                    if (casted_ability != nullptr) {
                        elems.push_back(casted_ability); 
                    } 
                }
            }
        }

        // Let AIs play
        for (auto ai : ais) {
            ai->play();
        }

        // Update elements
        std::chrono::duration<float, std::milli> elapsed = std::chrono::system_clock::now()
            - last_update;
        last_update = std::chrono::system_clock::now();
        for (auto elem : elems) {
            elem->update(elapsed.count());
        }

        std::vector<Collision> collisions = CollisionDetectionSystem::detect(elems);
        CollisionResolutionSystem::resolve(collisions);

        // Clear previous frame
        window.clear();

        // Draw elements
        for (auto i=0; i<elems.size(); i++) {
            // Garbage collector
            if (!elems[i]->isAlive()) {
                elems.erase(elems.begin() + i);
                continue;
            }
            window.draw(*elems[i]->getShape()->getDrawable());
        }

        // Debuffer the frame
        window.display();
    }

    return 0;
}
