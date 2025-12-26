cmake_minimum_required (VERSION 3.5)

project (test)


if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 14)
endif()
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    if (MSVC_VERSION GREATER 1910)
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -permissive-")
    endif ()
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Zc:rvalueCast -Zc:__cplusplus -EHsc")
    #set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Za") # do not disable language extensions because of boost
endif ()


include_directories (../src)

file(GLOB JSON-CORE-SRC src/json/json.core-*.cxx)
add_executable (cxon-json ${JSON-CORE-SRC})
set_target_properties (
    cxon-json PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

file(GLOB JSON-NODE-SRC src/json/json.node-*.cxx)
add_executable (cxon-json-node ${JSON-NODE-SRC})
set_target_properties (
    cxon-json-node PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

enable_testing()

add_test (
    NAME cxon/json/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-json
)
add_test (
    NAME cxon/json/node/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-json-node
)
add_test (
    NAME cxon/json/node/pass
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-json-node ../data/json/tests.cf pass
)
add_test (
    NAME cxon/json/node/fail
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-json-node ../data/json/tests.cf fail
)

find_program (CMAKE_BASH bash)
find_program (CMAKE_XARGS xargs)
find_program (CMAKE_DIFF diff)
find_program (CMAKE_MAKE make)

if (CMAKE_BASH AND CMAKE_XARGS AND CMAKE_DIFF)
    add_test (
        NAME cxon/json/node/diff-node
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        COMMAND ${CMAKE_BASH} -c "${PROJECT_SOURCE_DIR}/.out/cxon-json-node ../data/json/tests.cf diff-node   | \"${CMAKE_XARGS}\" -n2 \"${CMAKE_DIFF}\" -q"
    )
    add_test (
        NAME cxon/json/node/diff-native
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        COMMAND ${CMAKE_BASH} -c "${PROJECT_SOURCE_DIR}/.out/cxon-json-node ../data/json/tests.cf diff-native | \"${CMAKE_XARGS}\" -n2 \"${CMAKE_DIFF}\" -q"
    )
endif ()

add_test (
    NAME examples
    COMMAND ${CMAKE_MAKE} -C ${PROJECT_SOURCE_DIR}/../examples check
)
