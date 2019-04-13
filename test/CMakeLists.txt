cmake_minimum_required (VERSION 3.1)

project (test)
 

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    if (MSVC_VERSION GREATER 1910)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -permissive-")
    endif ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Za -Zc:rvalueCast")
endif ()


include_directories (..)

add_executable (
    cxon cxon.cxx
)
set_target_properties (
    cxon PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
)

add_executable (
    cxjson
    cxjson.cxx
)
set_target_properties (
    cxjson PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
)
 

enable_testing()

add_test (
    NAME cxon/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon
)
add_test (
    NAME cxjson/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxjson
)
add_test (
    NAME cxjson/pass|fail
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxjson pass @../data/set.1-pass.in @../data/set.2-pass.in fail @../data/set.1-fail.in @../data/set.2-fail.in
)

find_program (CMAKE_BASH bash)
find_program (CMAKE_XARGS xargs)
find_program (CMAKE_DIFF diff)

if (CMAKE_SH AND CMAKE_XARGS AND CMAKE_DIFF)
    add_test (
        NAME cxjson/diff
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        COMMAND ${CMAKE_BASH} -c "${PROJECT_SOURCE_DIR}/.out/cxjson diff @../data/set.3-diff.in @../data/set.4-diff.in @../data/set.5-diff.in | ${CMAKE_XARGS} -n2 ${CMAKE_DIFF} -q"
    )
endif ()
