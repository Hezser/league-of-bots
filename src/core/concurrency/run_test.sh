#!/bin/bash
g++ -g -c ./*.cpp &&
g++ -g ./*.o -o concurrency-test -pthread &&
./concurrency-test
