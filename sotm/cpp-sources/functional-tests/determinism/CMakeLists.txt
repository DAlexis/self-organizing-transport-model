project(functional-tests-determinism)

configure_file(determinism-test.sh determinism-test.sh COPYONLY)
configure_file(det-test.conf det-test.conf COPYONLY)

add_functional_test(determinism-test.sh .)
