#!/bin/bash
#rm -Rf cmake-build
cmake -B cmake-build -G Ninja .
cd cmake-build
ninja
