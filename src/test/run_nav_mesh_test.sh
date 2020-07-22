#!/bin/bash
g++ -g -c ./nav_mesh_test.cpp ../core/physics/*.cpp ../core/logic/elements/*.cpp ../core/logic/ai/*.cpp &&
g++ -g ./*.o -o nav_mesh-test -lsfml-graphics -lsfml-window -lsfml-system &&
./nav_mesh-test
