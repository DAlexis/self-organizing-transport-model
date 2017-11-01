cmake_minimum_required(VERSION 2.8)

project(functional-tests-determinism)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

configure_file(determinism-test.sh determinism-test.sh COPYONLY)
configure_file(det-test.conf det-test.conf COPYONLY)

add_functional_test(determinism-test.sh .)

