cmake_minimum_required(VERSION 3.20)

add_executable(SoftwareTracer
                main.cpp
                render.cpp
                render.h
                ObjReader.cpp
                ObjReader.h
             )

target_link_libraries(SoftwareTracer
        PRIVATE
        thirdparty-pngpp
        thirdparty-CImg
        thirdparty-CMarkup
        Utility
        Stats
        Samplers
        GObjects
        Deserialize
        RenderTools)

target_compile_definitions(SoftwareTracer PRIVATE -DRootPath="${CMAKE_SOURCE_DIR}")