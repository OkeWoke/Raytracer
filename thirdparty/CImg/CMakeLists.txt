cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
        cimg
        GIT_REPOSITORY https://github.com/GreycLab/CImg
        GIT_TAG v.3.3.2
)
FetchContent_MakeAvailable(cimg)

add_library(thirdparty-CImg INTERFACE)

target_include_directories(thirdparty-CImg INTERFACE  ${cimg_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR} )

if(APPLE OR UNIX)
    find_package(X11)
    target_link_libraries(thirdparty-CImg INTERFACE ${X11_LIBRARIES})
endif()