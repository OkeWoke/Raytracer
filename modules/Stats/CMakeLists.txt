cmake_minimum_required(VERSION 3.20)

add_library(Stats stats.cpp)


target_include_directories(
        Stats
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)


