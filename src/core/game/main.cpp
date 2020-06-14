#include <chrono>
#include <vector>
#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include "../logic/elements/elem.hpp"
#include "../logic/elements/bot.hpp"
#include "../logic/elements/ability.hpp"

int main() {
    // TODO: Use tai_clock when C++20 is released; system_clock can be altered by changing the time of the system
    std::chrono::system_clock::time_point last_update = std::chrono::system_clock::now();
    std::vector<Elem*> elems;
    // For now, we have one bot
    SaiBot* sai = new SaiBot(white_team, {0, 0});
    elems.push_back(sai);

    sf::RenderWindow window(sf::VideoMode(200, 200), "Loading...");

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
            }
            else if (event.type == sf::Event::MouseButtonPressed) {
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

        // Update elements
        std::chrono::duration<float, std::milli> ms = std::chrono::system_clock::now() - last_update;
        last_update = std::chrono::system_clock::now();
        for (auto elem : elems) {
            elem->update(ms.count());
        }

        // Clear previous frame
        window.clear();

        // Draw elements
        for (auto i=0; i<elems.size(); i++) {
            // Garbage collector
            if (!elems[i]->isAlive()) {
                elems.erase(elems.begin() + i);
                continue;
            }
            switch (elems[i]->getType()) {
                case bot_t: {
                    sf::RectangleShape bot_mesh;
                    bot_mesh.setSize(sf::Vector2f(20, 20));
                    bot_mesh.setOutlineColor(sf::Color::Red);
                    bot_mesh.setOutlineThickness(2);
                    bot_mesh.setPosition(elems[i]->getCoord()[0], elems[i]->getCoord()[1]);
                    window.draw(bot_mesh);
                    break;
                }
                case ability_t: {
                    sf::RectangleShape ability_mesh;
                    ability_mesh.setSize(sf::Vector2f(5, 5));
                    ability_mesh.setOutlineColor(sf::Color::Green);
                    ability_mesh.setOutlineThickness(1);
                    ability_mesh.setPosition(elems[i]->getCoord()[0], elems[i]->getCoord()[1]);
                    window.draw(ability_mesh);
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
