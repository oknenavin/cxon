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
endif()


set (CXON_OUT ${PROJECT_SOURCE_DIR}/.out)

include_directories (..)

add_executable (cxon
    cxon.cxx)
add_custom_command (TARGET cxon POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:cxon> ${CXON_OUT})

add_executable (cxjson
    cxjson.cxx)
add_custom_command (TARGET cxjson POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:cxjson> ${CXON_OUT})
 

enable_testing()

add_test (cxon/self
    ${CXON_OUT}/cxon)
add_test (cxjson/self
    ${CXON_OUT}/cxjson)
add_test (cxjson/pass|fail
    ${CXON_OUT}/cxjson pass @../data/set.1-pass.in @../data/set.2-pass.in fail @../data/set.1-fail.in @../data/set.2-fail.in)

find_program (CMAKE_SH sh)
find_program (CMAKE_DIFF diff)
find_program (CMAKE_XARGS xargs)

if (CMAKE_SH AND CMAKE_DIFF AND CMAKE_XARGS)
    add_test (cxjson/diff
        ${CMAKE_SH} -c "${CXON_OUT}/cxjson diff @../data/set.3-diff.in @../data/set.4-diff.in @../data/set.5-diff.in | ${CMAKE_XARGS} -n2 ${CMAKE_DIFF} -q")
endif ()
