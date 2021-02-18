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


include_directories (../src)

add_executable (
    cxon.json src/json/json.json.cxx src/json/json.fundamental.cxx src/json/json.compound.cxx src/json/json.std.cxx
)
set_target_properties (
    cxon.json PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

add_executable (
    cxon.json.node src/json/json.node.cxx
)
set_target_properties (
    cxon.json.node PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

add_executable (
    cxon.cbor src/cbor/cbor.cbor.cxx src/cbor/cbor.fundamental.cxx src/cbor/cbor.compound.cxx src/cbor/cbor.std.cxx
)
set_target_properties (
    cxon.cbor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)
 

enable_testing()

add_test (
    NAME cxon/json/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon.json
)
add_test (
    NAME cxon/json/node/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon.json.node
)
add_test (
    NAME cxon/json/node/pass
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon.json.node pass @../data/set.1-pass.in @../data/set.2-pass.in
)
add_test (
    NAME cxon/json/node/fail
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon.json.node fail @../data/set.1-fail.in @../data/set.2-fail.in
)

find_program (CMAKE_BASH bash)
find_program (CMAKE_XARGS xargs)
find_program (CMAKE_DIFF diff)

if (CMAKE_BASH AND CMAKE_XARGS AND CMAKE_DIFF)
    add_test (
        NAME cxon/json/node/diff
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        COMMAND ${CMAKE_BASH} -c "${PROJECT_SOURCE_DIR}/.out/cxon.json.node diff @../data/set.3-diff.in @../data/set.4-diff.in @../data/set.5-diff.in | \"${CMAKE_XARGS}\" -n2 \"${CMAKE_DIFF}\" -q"
    )
endif ()

add_test (
    NAME cxon/cbor/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon.cbor
)
