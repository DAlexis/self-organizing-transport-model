cmake_minimum_required(VERSION 2.8)

project(gui-test-animated-demo-graph)

include_directories (
    ${sotm_INCLUDE_DIRS}
    ${sotm-gui_INCLUDE_DIRS}
    ${unit-tests_INCLUDE_DIRS}
)

set(EXE_SOURCES
    animated-demo-graph-gui.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_link_libraries (${PROJECT_NAME} PUBLIC
    sotm
    sotm-gui
) 
