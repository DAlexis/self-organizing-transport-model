cmake_minimum_required(VERSION 2.8)

project(gui-test-static-graph)

set(EXE_SOURCES
    static-graph-gui.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PUBLIC
    sotm
    sotm-gui
) 
