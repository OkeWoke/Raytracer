cmake_minimum_required(VERSION 3.20)

add_library(GObjects
            Mesh.cpp
            Plane.cpp
            Sphere.cpp
            Triangle.cpp
            BoundVolume.cpp
            BoundVolumeHierarchy.cpp
            )

    
target_include_directories(
    GObjects
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(GObjects PRIVATE thirdparty-CMarkup thirdparty-pngpp)
target_link_libraries(GObjects PUBLIC Utility Stats)
target_compile_definitions(GObjects PRIVATE -DRootPath="${CMAKE_SOURCE_DIR}")
