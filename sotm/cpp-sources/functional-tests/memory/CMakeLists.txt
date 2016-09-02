cmake_minimum_required(VERSION 2.8)

project(functional-tests-memory)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

configure_file(leak-test.sh leak-test.sh COPYONLY)

add_functional_test(leak-test.sh simple-graph/simple-graph-memory-leak-test)

add_subdirectory(simple-graph)
