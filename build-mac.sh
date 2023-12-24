#!/bin/bash
rm -Rf cmake-build
cmake -B cmake-build .
cd cmake-build
make
