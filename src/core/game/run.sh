#!/bin/bash
g++ -g -c ./*.cpp ../logic/elements/*.cpp ../logic/ai/*.cpp ../physics/*.cpp &&
g++ -g ./*.o -o league-of-bots -pthread -lsfml-graphics -lsfml-window -lsfml-system &&
./league-of-bots
