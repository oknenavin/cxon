cmake_minimum_required (VERSION 3.1)

project (test)


if (NOT DEFINED CMAKE_CXX_STANDARD)
    set (CMAKE_CXX_STANDARD 11)
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

add_executable (
    cxon-json
        src/json/json.core-main.cxx
        src/json/json.core-fundamental.cxx
        src/json/json.core-compound.cxx
        src/json/json.core-std.cxx
        src/json/json.core-boost.cxx
)
set_target_properties (
    cxon-json PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

add_executable (
    cxon-json-node
        src/json/json.node-main.cxx
        src/json/json.node-self.cxx
        src/json/json.node-time.node.cxx
        src/json/json.node-time.native.cxx
        src/json/json.node-time.boost.cxx
        src/json/json.node-time.rapidjson.cxx
        src/json/json.node-time.nlohmann.cxx
)
set_target_properties (
    cxon-json-node PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

add_executable (
    cxon-cbor
        src/cbor/cbor.core-main.cxx
        src/cbor/cbor.core-fundamental.cxx
        src/cbor/cbor.core-compound.cxx
        src/cbor/cbor.core-std.cxx
        src/cbor/cbor.core-boost.cxx
)
set_target_properties (
    cxon-cbor PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/.out
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/.out
)

add_executable (
    cxon-cbor-node
        src/cbor/cbor.node-main.cxx
        src/cbor/cbor.node-self.cxx
)
set_target_properties (
    cxon-cbor-node PROPERTIES
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

if (CMAKE_BASH AND CMAKE_XARGS AND CMAKE_DIFF)
    add_test (
        NAME cxon/json/node/diff
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        COMMAND ${CMAKE_BASH} -c "${PROJECT_SOURCE_DIR}/.out/cxon-json-node ../data/json/tests.cf diff | \"${CMAKE_XARGS}\" -n2 \"${CMAKE_DIFF}\" -q"
    )
endif ()

add_test (
    NAME cxon/cbor/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-cbor
)

add_test (
    NAME cxon/cbor/node/self
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-cbor-node
)
add_test (
    NAME cxon/cbor/node/test-vector
    COMMAND ${PROJECT_SOURCE_DIR}/.out/cxon-cbor-node ../data/cbor/tests.cf test-vector
)

if (CMAKE_BASH AND CMAKE_XARGS AND CMAKE_DIFF)
    add_test (
        NAME cxon/cbor/node/round-trip
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/.out
        COMMAND ${CMAKE_BASH} -c "${PROJECT_SOURCE_DIR}/.out/cxon-cbor-node ../data/cbor/tests.cf round-trip | \"${CMAKE_XARGS}\" -n2 \"${CMAKE_DIFF}\" -q"
    )
endif ()
