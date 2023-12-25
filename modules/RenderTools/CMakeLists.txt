cmake_minimum_required(VERSION 3.20)

add_library(RenderTools
    Camera.cpp
    Light.cpp
    imageArray.cpp)

target_include_directories(RenderTools PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RenderTools PRIVATE Utility thirdparty-CMarkup)