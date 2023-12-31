cmake_minimum_required(VERSION 3.20)

set(PROJECT_NAME "GObjects")
add_library(${PROJECT_NAME}
            Mesh.cpp
            Plane.cpp
            Sphere.cpp
            Triangle.cpp
            BoundVolume.cpp
            BoundVolumeHierarchy.cpp
            )

    
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE thirdparty-CMarkup thirdparty-pngpp)
target_link_libraries(${PROJECT_NAME} PUBLIC Utility Stats)
#target_compile_definitions(${PROJECT_NAME} PRIVATE -DRootPath="${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
set(PROJECT_TEST_NAME "${PROJECT_NAME}-tests")
add_executable(${PROJECT_TEST_NAME} ${TEST_SOURCE})
target_link_libraries(${PROJECT_TEST_NAME}  PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
catch_discover_tests(${PROJECT_TEST_NAME})

