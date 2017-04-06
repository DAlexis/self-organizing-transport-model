cmake_minimum_required(VERSION 2.8)

project(sotm-unit-tests)

find_package (GTest REQUIRED)

include_directories (
    ${GTEST_INCLUDE_DIR}
    ${sotm_INCLUDE_DIRS}
)

set(EXE_SOURCES
    math/generic-ut.cpp
    math/geometry-ut.cpp
    math/integration-ut.cpp
    math/distrib-gen-ut.cpp
    math/field-ut.cpp
    math/functions-ut.cpp
    base/transport-graph-ut.cpp
    output/variables-ut.cpp
    utils/memory-ut.cpp
    payloads/demo/empty-payload-ut.cpp
    time-iter/euler-explicit-ut.cpp
    time-iter/runge-kutta-ut.cpp
    time-iter/exponent-time-iterable.cpp
    complex/branching-ut.cpp
)

set(EXE_HPP
    complex/branching-trivial-physics.hpp
    time-iter/exponent-time-iterable.hpp
    complex/branching-trivial-physics.hpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES} ${EXE_HPP})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
    ${sotm_LIBRARY}
)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)


add_test(NAME Library_testing
         COMMAND ${PROJECT_NAME})

add_unit_test(${PROJECT_NAME})
