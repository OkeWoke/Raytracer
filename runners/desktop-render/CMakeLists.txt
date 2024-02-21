cmake_minimum_required(VERSION 3.20)


add_executable(SoftwareTracer
                main.cpp
                deserialize.h
                deserialize.cpp
             )

target_link_libraries(SoftwareTracer
        PRIVATE
        thirdparty-CImg
        Render)

target_compile_definitions(SoftwareTracer PRIVATE -DRootPath="${CMAKE_SOURCE_DIR}")