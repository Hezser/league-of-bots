#include <vector>
#include <iostream>
#include <thread>
#include <SFML/Graphics.hpp>
#include "../core/physics/nav_mesh.hpp"
#include "../core/physics/shape.hpp"
#include "../core/logic/elements/terrain.hpp"

void drawTerrain(sf::RenderWindow& window, std::vector<Terrain*> terrains) {
    for (Terrain* t : terrains) {
        sf::Shape* drawable = t->getShape()->getDrawable();  // TODO: Implement
        window.draw(*drawable);
    }
}

void drawNavMesh(sf::RenderWindow& window, TriangleMesh mesh) {
    for (Triangle* t : mesh) {
        sf::Drawable* drawable = t->getDrawable();  // TODO: Implement
        window.draw(*drawable);
    }
}

int main() {
    // Create terrain
    MapSize map_size = {1920, 1080};
    std::vector<Terrain*> terrains;
    for (auto i=0; i<map_size.y; i += 200) {
        ConvexPolygon* s = new ConvexPolygon({{i, i}, {i, i+50}, {i+50, i+50}, {i+50, i}});
        Terrain* t = new Terrain(s, {i+25, i+25}, 71);
        terrains.push_back(t);
    }

    sf::RenderWindow window(sf::VideoMode(map_size.x, map_size.y), "Loading...");
    while (window.isOpen()) {
        // Display naked map for a few seconds
        drawTerrain(window, terrains);
        window.display();
        std::this_thread::sleep_for(std::chrono::seconds(0));
        
        // Triangulate map
        NavMesh* nav_mesh;
        try {
            nav_mesh = new NavMesh(terrains, map_size);
        } catch (NavMesh::InsufficientNodesException e) {
            std::cout << e.what() << std::endl;
            return 0;
        } catch (NavMesh::FailedTriangulationException e) {
            std::cout << e.what() << std::endl;
            return 0;
        }

        // Display triangulation
        window.clear();
        drawTerrain(window, terrains);
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
