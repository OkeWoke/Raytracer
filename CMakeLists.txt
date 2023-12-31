cmake_minimum_required(VERSION 3.20)
project(Raytracer C CXX)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "{CMAKE_CXX_FLAGS} -Wall -Wextra")

if(MSVC)
include(${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake)
endif()
include(CTest)
add_subdirectory(thirdparty)
add_subdirectory(modules)
add_subdirectory(runners)