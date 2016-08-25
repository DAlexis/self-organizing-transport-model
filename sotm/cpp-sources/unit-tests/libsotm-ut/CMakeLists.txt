cmake_minimum_required(VERSION 2.8)

project(sotm-unit-tests)

find_package (GTest REQUIRED)

include_directories (
    ${GTEST_INCLUDE_DIR}
    ${sotm_INCLUDE_DIRS}
)

set(EXE_SOURCES
    math/geometry-ut.cpp
    utils/memory-ut.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    gtest
    gtest_main
    ${sotm_LIBRARY}
)

add_test(NAME Library_testing
         COMMAND ${PROJECT_NAME})

run_test(${PROJECT_NAME})
