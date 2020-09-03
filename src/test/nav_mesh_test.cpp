/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#include <vector>
#include <thread>
#include <SFML/Graphics.hpp>
#include "../core/graphics/nav_mesh.hpp"
#include "../core/logic/elements/terrain.hpp"

using namespace adamant::logic::elements;
using namespace adamant::graphics;
using namespace adamant::graphics::elements;

void drawTerrain(sf::RenderWindow& window, std::vector<Terrain*> terrains) {
    for (auto i=0; i<terrains.size(); i++) { 
        std::vector<Node*> nodes = terrains[i]->getShape()->nodes;
        sf::ConvexShape convex;
        convex.setPointCount(nodes.size());
        for (auto i=0; i<nodes.size(); i++) {
            convex.setPoint(i, sf::Vector2f(nodes[i]->coord.x, nodes[i]->coord.y));
        }
        convex.setFillColor(sf::Color::White);
        window.draw(convex);
    }
}

void drawNodes(sf::RenderWindow& window, std::vector<Node*> nodes) {
    for (Node* n : nodes) {
        sf::CircleShape s = sf::CircleShape(5.0);
        s.setPosition(n->coord.x, n->coord.y);
        s.setFillColor(sf::Color::Green);
        window.draw(s);
    }
}

void drawTriangle(sf::RenderWindow& window, Triangle* t, sf::Color outline_color) {
    std::vector<Node*> nodes = t->nodes;
    sf::ConvexShape convex;
    convex.setPointCount(nodes.size());
    for (auto i=0; i<nodes.size(); i++) {
        convex.setPoint(i, sf::Vector2f(nodes[i]->coord.x, nodes[i]->coord.y));
    }
    convex.setFillColor(sf::Color::Transparent);
    convex.setOutlineColor(outline_color);
    convex.setOutlineThickness(2.0);
    window.draw(convex);
}

void drawNavMesh(sf::RenderWindow& window, TriangleMesh mesh) {
    for (auto i=0; i<mesh.size(); i++) {
        if (i == mesh.size()-1) drawTriangle(window, mesh[i], sf::Color::Red);
        else drawTriangle(window, mesh[i], sf::Color::Green);
    }
}

int main() {
    MapSize map_size = {750, 750};
    std::vector<Terrain*> terrains;

    // Staircase terrain
    for (auto i=0; i<map_size.y; i += 100) {
        ConvexPolygon* s = new ConvexPolygon({{i, i}, {i, i+50}, {i+50, i+50}, {i+50, i}});
        Terrain* t = new Terrain(s, {i+25, i+25}, 71);
        terrains.push_back(t);
    }

    // Random terrain
    /* for (auto x=0; x<map_size.x; x+= 100) { */
    /*     for (auto y=0; y<map_size.y; y+=100) { */
    /*         int rnd_x_1 = 0; */
    /*         int rnd_x_2 = 0; */
    /*         int rnd_y_1 = 0; */
    /*         int rnd_y_2 = 0; */
    /*         // Sensible dimensions */
    /*         while ((rnd_x_2 - rnd_x_1 < 5) || (rnd_y_2 - rnd_y_1 < 5)) { */
    /*             int tmp_x_1 = rand() % 100; */
    /*             int tmp_x_2 = rand() % 100; */
    /*             int tmp_y_1 = rand() % 100; */
    /*             int tmp_y_2 = rand() % 100; */
    /*             if (tmp_x_1 < tmp_x_2) { */
    /*                 rnd_x_1 = tmp_x_1; */
    /*                 rnd_x_2 = tmp_x_2; */
    /*             } else { */
    /*                 rnd_x_1 = tmp_x_2; */
    /*                 rnd_x_2 = tmp_x_1; */
    /*             } */
    /*             if (tmp_y_1 < tmp_y_2) { */
    /*                 rnd_y_1 = tmp_y_1; */
    /*                 rnd_y_2 = tmp_y_2; */
    /*             } else { */
    /*                 rnd_y_1 = tmp_y_2; */
    /*                 rnd_y_2 = tmp_y_1; */
    /*             } */
    /*         } */
    /*         ConvexPolygon* s = new ConvexPolygon({{x+rnd_x_1, y+rnd_y_1}, {x+rnd_x_1, y+rnd_y_2}, */
    /*                 {x+rnd_x_2, y+rnd_y_2}, {x+rnd_x_2, y+rnd_y_1}}); */
    /*         Terrain* t = new Terrain(s, {x+(rnd_x_2-rnd_x_1), y+(rnd_y_2-rnd_y_1)}, 0); */
    /*         terrains.push_back(t); */
    /*     } */
    /* } */

    sf::RenderWindow window(sf::VideoMode(map_size.x, map_size.y), "Loading...");
    while (window.isOpen()) {
        // Display naked map for a few seconds
        std::vector<Node*> nodes;
        drawTerrain(window, terrains);
        for (auto t : terrains) {
            for (auto n : t->getShape()->nodes) {
                nodes.push_back(n);
            }
        }
        drawNodes(window, nodes);

        window.display();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        
        // Triangulate map
        NavMesh* nav_mesh;
        try {
            nav_mesh = new NavMesh(terrains, map_size);
        } catch (NavMesh::InsufficientNodesException e) {
            return 0;
        } catch (NavMesh::FailedTriangulationException e) {
            return 0;
        }

        // Display triangulation
        std::vector<Triangle*> mesh = nav_mesh->getMesh();
        for (auto i=0; i<mesh.size(); i++) {
            window.clear();
            drawTerrain(window, terrains);
            drawNavMesh(window, std::vector<Triangle*>(mesh.begin(), mesh.begin() + i+1));
            /* drawNodes(window, nav_mesh->getNodes()); */
            window.display();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // Wait to close the window
        sf::Event event;
        while(true) {
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    // Garbage collector
                    delete nav_mesh;  // TODO: Make NavMesh destructor also destruct its members
                    for (auto t : terrains) {
                        delete t;  // TODO: Make Terrain destructor also destruct its members
                    }
                    window.close();
                    return 0;
                }
            }
        }
    }

    return 0;
}
