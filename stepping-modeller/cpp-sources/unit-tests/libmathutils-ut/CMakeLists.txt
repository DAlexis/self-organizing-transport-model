cmake_minimum_required(VERSION 2.8)

project(libstepmod-unit-tests)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(geometry-ut)
