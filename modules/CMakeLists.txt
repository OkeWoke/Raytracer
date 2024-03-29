cmake_minimum_required(VERSION 3.20)

macro(render_module name)
    message(" -------------------------------------------------------")
    message("Creating module: ${name}")

    set(PROJECT_NAME ${name})
    file(GLOB PROJECT_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
    add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE})
    target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

    set (extra_args ${ARGN})
    if(extra_args)
        foreach(arg IN LISTS extra_args)
            message("Linking: ${arg}")
            target_link_libraries(${PROJECT_NAME} PUBLIC ${arg}) # Currently PUBLIC to avoid duplicate libs. Not the best solution.
        endforeach()
    endif()

    file(GLOB_RECURSE TEST_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp")
    if(TEST_SOURCE)
        message("Creating tests for ${PROJECT_NAME}")
        set(PROJECT_TEST_NAME "${PROJECT_NAME}-tests")
        add_executable(${PROJECT_TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${PROJECT_TEST_NAME} PRIVATE Catch2::Catch2WithMain ${PROJECT_NAME})
        if(extra_args)
            foreach(arg IN LISTS extra_args)
                message("Linking Test: ${arg}")
                target_link_libraries(${PROJECT_TEST_NAME} PUBLIC ${arg}) # Currently PUBLIC to avoid duplicate libs. Not the best solution.
            endforeach()
        endif()
        catch_discover_tests(${PROJECT_TEST_NAME})
        target_compile_definitions(${PROJECT_TEST_NAME} PRIVATE -DTEST_DATA_PATH="${CMAKE_CURRENT_SOURCE_DIR}/tests/test_data/")
    else()
        message("No tests for ${PROJECT_NAME}")
    endif()
message(" -------------------------------------------------------")
endmacro()

add_subdirectory(Stats)
add_subdirectory(Utility)
add_subdirectory(GObjects)
add_subdirectory(Samplers)
add_subdirectory(RenderTools)
add_subdirectory(Render)

