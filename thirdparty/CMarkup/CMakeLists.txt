cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
        cmarkup
        URL https://www.firstobject.com/Markup115.zip
        DOWNLOAD_EXTRACT_TIMESTAMP FALSE
)
FetchContent_MakeAvailable(cmarkup)

add_library(thirdparty-CMarkup ${cmarkup_SOURCE_DIR}/Markup.cpp)
#set_source_files_properties(${cmarkup_SOURCE_DIR}/Markup.cpp PROPERTIES COMPILE_FLAGS /Y-)
target_include_directories(
    thirdparty-CMarkup
    PUBLIC
     ${cmarkup_SOURCE_DIR}
)

target_include_directories(
    thirdparty-CMarkup
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(thirdparty-CMarkup PRIVATE "-Wno-deprecated-declarations")
    find_package(Iconv REQUIRED)
    target_link_libraries(thirdparty-CMarkup PRIVATE Iconv::Iconv)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_definitions(thirdparty-CMarkup PRIVATE MARKUP_STL MARKUP_WINCONV)
    target_compile_options(thirdparty-CMarkup PRIVATE "/wd4996")
endif()


