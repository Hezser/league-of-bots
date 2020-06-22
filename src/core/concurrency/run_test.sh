#!/bin/bash
g++ -g -c ./*.cpp &&
g++ -g ./*.o -o concurrency-test -pthread -lboost_fiber -lboost_context &&
./concurrency-test
