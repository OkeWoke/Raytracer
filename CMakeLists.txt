cmake_minimum_required(VERSION 3.20)
project(Raytracer C CXX)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "{CMAKE_CXX_FLAGS} -Wall -Wextra")

add_subdirectory(thirdparty)
add_subdirectory(modules)
