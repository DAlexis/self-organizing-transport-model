cmake_minimum_required(VERSION 2.8)

project(gui-test-electrostatics)

set(EXE_SOURCES
    electrostatics-gui.cpp
)

add_executable(${PROJECT_NAME} ${EXE_SOURCES})

target_include_directories (${PROJECT_NAME} PUBLIC
    ${unit-tests_INCLUDE_DIRS}
)


target_link_libraries (${PROJECT_NAME} PUBLIC
    sotm
    sotm-gui
) 
