cmake_minimum_required(VERSION 2.8)

project(functional-tests-gui)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(static-graph)
add_subdirectory(animated-demo-graph)

