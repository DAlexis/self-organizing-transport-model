cmake_minimum_required(VERSION 2.8)

project(simple-graph-memory-leak-test)

include_directories (
    ${sotm_INCLUDE_DIRS}
)

set(EXE_SOURCES
    simple-graph.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME}
    ${sotm_LIBRARY}
)