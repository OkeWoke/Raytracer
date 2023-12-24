cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    pngpp
    URL https://download.savannah.nongnu.org/releases/pngpp/png++-0.2.9.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_GetProperties(pngpp)
if(NOT pngpp_POPULATED)
    FetchContent_Populate(pngpp)
endif()

add_library(thirdparty-pngpp INTERFACE)


#target_sources(thirdparty-pngpp PRIVATE ${pngpp_SOURCE_DIR}/png++/png.hpp)
target_include_directories(thirdparty-pngpp INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${pngpp_SOURCE_DIR} )
target_link_libraries(thirdparty-pngpp INTERFACE thirdparty-libpng)

