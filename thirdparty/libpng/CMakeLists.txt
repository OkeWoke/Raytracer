cmake_minimum_required(VERSION 3.20)

if(MSVC)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")
endif()

find_package(PNG REQUIRED)

message(STATUS "libpng include dir: ${PNG_INCLUDE_DIRS}")
message(STATUS "libpng library dir: ${PNG_LIBRARIES}")

add_library(thirdparty-libpng INTERFACE)

target_include_directories(thirdparty-libpng INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${PNG_INCLUDE_DIRS})
target_link_libraries(thirdparty-libpng INTERFACE ${PNG_LIBRARIES})
message(STATUS "PNG_INCLUDE_DIRS: ${PNG_INCLUDE_DIRS}")
