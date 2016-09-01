cmake_minimum_required(VERSION 2.8)

project(functional-tests)

export_include_dirs(
    ${PROJECT_SOURCE_DIR}
)

configure_file(run-function-tests.sh run-function-tests.sh COPYONLY)

add_functional_test(pwd)

add_custom_target(qq_runtest
      COMMAND echo "qq"
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
  add_dependencies(run-tests qq_runtest)

add_subdirectory(memory)
