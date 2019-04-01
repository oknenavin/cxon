cmake_minimum_required (VERSION 3.1)

project (test)
 

set (CMAKE_CXX_STANDARD 11)

if (MSVC)
    if (MSVC_VERSION GREATER 1910)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Za -permissive-")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Za -Zc:rvalueCast")
endif()


include_directories (..)

add_executable (cxon
    cxon.cxx)
set_target_properties (cxon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out)

add_executable (cxjson
    cxjson.cxx)
set_target_properties (cxjson PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out)
 

enable_testing()

add_test (cxon/self
    ${PROJECT_SOURCE_DIR}/.out/cxon)
add_test (cxjson/self
    ${PROJECT_SOURCE_DIR}/.out/cxjson)
add_test (cxjson/pass|fail
    ${PROJECT_SOURCE_DIR}/.out/cxjson pass @../data/set.1-pass.in @../data/set.2-pass.in fail @../data/set.1-fail.in @../data/set.2-fail.in)
