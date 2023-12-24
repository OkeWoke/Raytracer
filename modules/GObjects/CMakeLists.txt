cmake_minimum_required(VERSION 3.20)

add_library(GObjects 
            GObject.cpp
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
    ${CURRENT_CMAKE_SOURCE_DIR}
)

target_link_libraries(GObjects PRIVATE thirdparty-CMarkup Utility thirdparty-pngpp)

