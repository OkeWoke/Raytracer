cmake_minimum_required(VERSION 3.20)

add_library(Samplers
            HaltonSampler.cpp
            RandomSampler.cpp
            Sampler.cpp)

target_include_directories(Samplers PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
