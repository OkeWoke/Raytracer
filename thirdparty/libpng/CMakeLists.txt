cmake_minimum_required(VERSION 3.20)

find_package(PNG)

if(NOT PNG_FOUND)
    message(STATUS "Fetching LibPNG via FetchContent from Github")
    include(FetchContent)

    FetchContent_Declare(
        libpng
        GIT_REPOSITORY https://github.com/glennrp/libpng.git
        GIT_TAG v1.6.37
    )

    FetchContent_Populate(libpng)
    
    set(PNG_INCLUDE_DIRS ${libpng_SOURCE_DIR})
    set(PNG_LIBRARIES png_static)  # Adjust library name based on the libpng build configuration
    #set(PNG_LIBRARIES png CACHE FILEPATH "png library" FORCE)
    message(STATUS "Fetch complete")
endif()

message(STATUS "libpng binary dir: ${PNG_INCLUDE_DIRS}")
message(STATUS "libpng binary dir: ${PNG_LIBRARIES}")

add_library(thirdparty-libpng INTERFACE)



target_include_directories(thirdparty-libpng INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${PNG_INCLUDE_DIRS})
target_link_libraries(thirdparty-libpng INTERFACE ${PNG_LIBRARIES})
message(STATUS "PNG_INCLUDE_DIRS: ${PNG_INCLUDE_DIRS}")
