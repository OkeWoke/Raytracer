cmake_minimum_required(VERSION 3.20)

add_executable(SoftwareTracer
main.cpp)

target_link_libraries(SoftwareTracer PRIVATE thirdparty-CImg thirdparty-CMarkup thirdparty-pngpp Samplers GObjects RenderTools)
