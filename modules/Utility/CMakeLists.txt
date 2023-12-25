cmake_minimum_required(VERSION 3.20)

add_library(Utility 
            Matrix.cpp
            Utility.cpp
            Vector.cpp
            Color.cpp
            )

    
target_include_directories(
    Utility
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Utility PRIVATE thirdparty-CMarkup)

