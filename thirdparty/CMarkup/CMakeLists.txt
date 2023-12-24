cmake_minimum_required(VERSION 3.20)

add_library(thirdparty-CMarkup thirdparty/Markup.cpp)

target_include_directories(
    thirdparty-CMarkup
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty
)

target_include_directories(
    thirdparty-CMarkup
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)