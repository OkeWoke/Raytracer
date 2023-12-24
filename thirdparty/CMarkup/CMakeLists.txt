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

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(thirdparty-CMarkup PRIVATE "-Wno-deprecated-declarations")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(thirdparty-CMarkup PRIVATE "/wd4996")
endif()

find_package(Iconv REQUIRED)
target_link_libraries(thirdparty-CMarkup PRIVATE Iconv::Iconv)