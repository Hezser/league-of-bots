#include <vector>
#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include "../core/physics/nav_mesh.hpp"
#include "../core/physics/shape.hpp"
#include "../core/logic/elements/terrain.hpp"

void drawTerrain(sf::RenderWindow& window, std::vector<Terrain*> terrains) {
    for (Terrain* t : terrains) {
        sf::Shape* drawable = t->getShape()->getDrawable();
        window.draw(*drawable);
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

void drawNavMesh(sf::RenderWindow& window, TriangleMesh mesh) {
    for (Triangle* t : mesh) {
        sf::Shape* drawable = t->getDrawable();
        drawable->setFillColor(sf::Color::Transparent);
        drawable->setOutlineColor(sf::Color::Red);
        drawable->setOutlineThickness(2.0);
        window.draw(*drawable);
    }
}

int main() {
    // Create terrain
    MapSize map_size = {500, 500};
    std::vector<Terrain*> terrains;
    for (auto i=0; i<map_size.y; i += 100) {
        ConvexPolygon* s = new ConvexPolygon({{i, i}, {i, i+50}, {i+50, i+50}, {i+50, i}});
        Terrain* t = new Terrain(s, {i+25, i+25}, 71);
        terrains.push_back(t);
    }

    sf::RenderWindow window(sf::VideoMode(map_size.x, map_size.y), "Loading...");
    while (window.isOpen()) {
        // Display naked map for a few seconds
        std::vector<Node*> nodes;
        drawTerrain(window, terrains);
        for (auto t : terrains) {
            for (auto e : t->getShape()->edges) {
                nodes.push_back(e->a);
                nodes.push_back(e->b);
            }
        }
        drawNodes(window, nodes);

        sf::CircleShape s = sf::CircleShape(20.0);
        s.setPosition(20, 400);
        s.setFillColor(sf::Color::Magenta);
        window.draw(s);
        sf::RectangleShape t = sf::RectangleShape({20.0, 20.0});
        t.setPosition(20, 400);
        t.setFillColor(sf::Color::Yellow);
        window.draw(t);

        window.display();
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
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
        window.clear();
        drawTerrain(window, terrains);
        drawNodes(window, nav_mesh->getNodes());
        drawNavMesh(window, nav_mesh->getMesh());
        window.display();

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
                }
            }
        }
    }

    return 0;
}
