#!/bin/bash
rm -Rf cmake-build
cmake -B cmake-build -DCMAKE_MAKE_PROGRAM=mingw32-make -G "MinGW Makefiles" .
cd cmake-build
make 