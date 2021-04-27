project(functional-tests-memory)

configure_file(leak-test.sh leak-test.sh COPYONLY)

add_functional_test(leak-test.sh simple-graph/simple-graph-memory-leak-test)
add_functional_test(leak-test.sh branching/branching-memory-leak-test)

add_subdirectory(simple-graph)
add_subdirectory(branching)
