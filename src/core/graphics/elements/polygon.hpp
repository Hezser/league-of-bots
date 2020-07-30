/**
 * Copyright (C) Sergio Hernandez - All Rights Reserved
 * Author: Sergio Hernandez <contact.sergiohernandez@gmail.com>
 * Date  : 30.07.2020
 */

#ifndef POLYGON_HPP
#define POLYGON_HPP

#include "coord.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "shape.hpp"
#include <SFML/Graphics.hpp>

/* TODO: Polygons are not usable at the moment, as SFML does not support concave shapes
 *       However, there is no need for them now (DELETE?) */
class Polygon: public Shape {
    public:
        std::vector<Node*> nodes;
        std::vector<Edge*> edges;
        Polygon(std::vector<Node*> nodes, std::vector<Edge*> edges);
        ~Polygon();
        void setCenter(Coord center) override;
        void defineNeighboursFromCenter(Coord origin);
        sf::Shape* getDrawable() override;


        struct InsufficientNodesException: public std::exception {
            const char* what() const throw();
        };
    
    protected:
        Polygon(ShapeType subtype);  // Used for instantiating subclasses
        Coord findCenter(std::vector<Coord> coords);
};

#endif
