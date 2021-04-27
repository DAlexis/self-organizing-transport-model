cmake_minimum_required(VERSION 2.8)

project(branching-memory-leak-test)

include_directories (
    ${sotm_INCLUDE_DIRS}
    ${unit-tests_INCLUDE_DIRS}
)

set(EXE_SOURCES
    branching.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PUBLIC
    sotm
)
