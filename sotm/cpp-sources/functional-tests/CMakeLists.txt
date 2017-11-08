cmake_minimum_required(VERSION 2.8)

project(functional-tests)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

configure_file(coloring.sh coloring.sh COPYONLY)

add_subdirectory(memory)
add_subdirectory(gui)
add_subdirectory(determinism)
