cmake_minimum_required(VERSION 2.8)

project(unit-tests)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

add_subdirectory(libsotm-ut)
add_subdirectory(lightning-modeller-ut)
